#include <pronto/raster/io.h>
#include <random>
#include <string>

namespace pronto 
{
  namespace raster 
  {
    namespace detail 
    {
      GDALDataset* create_compressed_gdaldataset(
        const std::filesystem::path& path, int rows, int cols
        , GDALDataType datatype, int nBands)
      {
        GDALAllRegister();

        GDALDriverManager* m = GetGDALDriverManager();
        GDALDriver* driver = m->GetDriverByName("GTiff");

        char **papszOptions = NULL;
        // Commented out settings that are default anyway

        // papszOptions = CSLSetNameValue(papszOptions, "BLOCKXSIZE", "256");
        // papszOptions = CSLSetNameValue(papszOptions, "BLOCKYSIZE", "256");
        papszOptions = CSLSetNameValue(papszOptions, "TILED", "YES");
        // papszOptions = CSLSetNameValue(papszOptions, "INTERLEAVE", "BAND");
        papszOptions = CSLSetNameValue(papszOptions, "COMPRESS", "LZW");
        // papszOptions = CSLSetNameValue(papszOptions, "BIGTIFF", "YES");

        GDALDataset* dataset = driver->Create(path.string().c_str(), cols, rows
          , nBands, datatype, papszOptions);
        return dataset;
      }

      GDALDataset* create_standard_gdaldataset(
      const std::filesystem::path& path, int rows, int cols
      , GDALDataType datatype, int nBands)
      {
        GDALAllRegister();

        GDALDriverManager* m = GetGDALDriverManager();
        GDALDriver* driver = m->GetDriverByName("GTiff");

        char **papszOptions = NULL;
        // Commented out settings that are default anyway

        //papszOptions = CSLSetNameValue(papszOptions, "BLOCKXSIZE", "256");
        //papszOptions = CSLSetNameValue(papszOptions, "BLOCKYSIZE", "256");
        papszOptions = CSLSetNameValue(papszOptions, "TILED", "YES");
        //papszOptions = CSLSetNameValue(papszOptions, "INTERLEAVE", "BAND");
        papszOptions = CSLSetNameValue(papszOptions, "COMPRESS", "NONE");
        papszOptions = CSLSetNameValue(papszOptions, "BIGTIFF", "IF_NEEDED");

        GDALDataset* dataset = driver->Create(path.string().c_str(), cols, rows
          , nBands, datatype, papszOptions);
        return dataset;
      }

      GDALDataset* create_compressed_gdaldataset_from_model
        (const std::filesystem::path& path
        , const gdal_raster_view_base& model
        , GDALDataType datatype, int nBands )
      {
        int rows = model.rows();
        int cols = model.cols();

        GDALDataset* model_data_set = model.get_band()->GetDataset();
        GDALDataset* dataset = create_compressed_gdaldataset(path, rows, cols
          , datatype, nBands);

        if (dataset == NULL) return NULL;

        double gt_data[6] = { 0,0,0,0,0,0};
        double* geotransform = gt_data;
        CPLErr error_status = model.get_geo_transform(geotransform);
        dataset->SetGeoTransform(geotransform);
        dataset->SetProjection(model_data_set->GetProjectionRef());
        return dataset;
      }


      GDALDataset* create_standard_gdaldataset_from_model
         ( const std::filesystem::path& path
         , const gdal_raster_view_base& model
         , GDALDataType datatype, int nBands )
      {
        int rows = model.rows();
        int cols = model.cols();

        GDALDataset* model_data_set = model.get_band()->GetDataset();
        GDALDataset* dataset = create_standard_gdaldataset(path, rows, cols
          , datatype, nBands);

        if (dataset == NULL) return NULL;

        double gt_data[6] = { 0, 0, 0, 0, 0, 0};
        double* geotransform = gt_data;
        CPLErr error_status = model.get_geo_transform(geotransform);
        dataset->SetGeoTransform(geotransform);
        dataset->SetProjection(model_data_set->GetProjectionRef());

        return dataset;
      }

      std::filesystem::path get_unique_path(const std::filesystem::path& path)
      {

        const wchar_t hex[] = L"0123456789abcdef";
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<int> dis(0, 15);

        // convert path to wstring
        std::wstring s = path.wstring();

        // replace % for random hex number
        for (auto&& ch : s) {
          if (ch == L'%') {
            ch = hex[dis(gen)];
          }
        }
        return std::filesystem::path(s);
      }

      std::filesystem::path get_temp_tiff_path()
      {
        auto temp_path = std::filesystem::temp_directory_path();
        std::filesystem::path unique_temp_path_model = temp_path /= "%%%%-%%%%-%%%%-%%%%.tif";
        return get_unique_path(unique_temp_path_model);
      }

      gdal_band_and_dataset_deleter::gdal_band_and_dataset_deleter(GDALDataset* dataset, bool delete_files)
        : m_dataset(dataset), m_delete_files(delete_files) {
        if (!m_dataset) {
          // This is a logic error, the dataset should always be valid here.
          // In production, consider logging or asserting.
          throw std::runtime_error("GDALBandAndDatasetDeleter initialized with nullptr dataset.");
        }
      }

      void gdal_band_and_dataset_deleter::operator()(GDALRasterBand* band) const {
        // The band pointer might be null if the shared_ptr was reset, but
        // the dataset pointer captured during construction is what matters for cleanup.
        // We do not rely on band->GetDataset() here.
        if (m_dataset) {
          // Optionally update statistics before closing, if desired
          // (Your original code had this in dataset_closer, put it here if needed)
          // optionally_update_statistics(band);

          // Get file list *before* closing if we intend to delete files
          char** file_list = nullptr;
          if (m_delete_files) {
            file_list = m_dataset->GetFileList();
          }
          GDALDriver* driver = m_dataset->GetDriver();
          GDALClose(m_dataset); // Always close the dataset

          if (m_delete_files && file_list) {

            if (driver) {
              for (int i = 0; file_list[i] != nullptr; ++i) {
                // Check if file exists before trying to delete
                // This prevents errors if a file from the list was already gone
                if (std::filesystem::exists(file_list[i])) {
                  CPLErr err = driver->Delete(file_list[i]);
                  if (err != CE_None) {
                    // Log error, don't throw
                    fprintf(stderr, "Failed to delete file %s: %s\n", file_list[i], CPLGetLastErrorMsg());
                  }
                }
              }
            }
            else {
              fprintf(stderr, "Warning: Could not get GDAL driver for deletion.\n");
            }
            CSLDestroy(file_list);
          }
          else if (m_delete_files && !file_list) {
            // This might happen for in-memory datasets or if GetFileList fails
            fprintf(stderr, "Warning: Attempted to delete files but GetFileList returned null or empty list.\n");
          }
        }
        else {
          // This should ideally not happen if constructed correctly.
          // Log a critical error.
          fprintf(stderr, "Error: GDALBandAndDatasetDeleter called with nullptr m_dataset.\n");
          // Do not throw from a destructor/deleter.
        }
      }
   

 
      void optionally_update_statistics(GDALRasterBand* band)
      {
        if (band && band->GetAccess() == GA_Update) {

          double min, max, mean, stddev;
          auto try_statistics = band->GetStatistics(FALSE,
          FALSE,
          &min, &max, &mean, &stddev);
          // Only update statistics if rasterband thinks that they are up
          // to date
          if (try_statistics != CE_Warning)
          {
            band->ComputeStatistics(FALSE, &min, &max, &mean, &stddev
              , NULL, NULL);
            band->SetStatistics(min, max, mean, stddev);
          }
        }
      }
      std::shared_ptr<GDALDataset> open_dataset(
        const std::filesystem::path& path, access access)
      {
        GDALAllRegister();
        GDALDataset* dataset = (GDALDataset*)GDALOpen(path.string().c_str()
        , gdal_access(access));

        bool test = dataset->GetAccess() == gdal_access(access);
        if (!test)
        {
          std::cout << "GDALAccess mismatch: " << path << std::endl;
          throw(opening_raster_failed{});
        }
        assert(test);
        if (dataset == nullptr) {
          std::cout << "Could not read: " << path << std::endl;
          throw(opening_raster_failed{});
        }

        auto closer = [](GDALDataset* ds) {
          if(ds) GDALClose(ds);
          ds = nullptr;
        };

        return std::shared_ptr<GDALDataset>(dataset, closer);
      }

      std::shared_ptr<GDALRasterBand> open_band(std::shared_ptr<GDALDataset> dataset, int band)
      {
        if (dataset == nullptr) {
          throw(opening_raster_failed{});
        }

        GDALRasterBand* rasterband = dataset->GetRasterBand(band);

        // capture dataset by value: now it will not be deleted before the band
        auto closer = [dataset](GDALRasterBand* rb)
        {
          optionally_update_statistics(rb);
        };

        return std::shared_ptr<GDALRasterBand>(rasterband, closer);
      }
      // -- - Modified create_band function(pass path to deleter) -- -
      std::shared_ptr<GDALRasterBand> create_band(
          const std::filesystem::path & path, int rows, int cols,
          GDALDataType datatype, is_temporary is_temp)
      {
        int nBands = 1;
        GDALDataset* dataset = create_standard_gdaldataset(path, rows, cols, datatype, nBands);

        if (dataset == nullptr) {
          throw creating_a_raster_failed{};
        }

        GDALRasterBand* band = dataset->GetRasterBand(1);
        if (band == nullptr) {
          GDALClose(dataset);
          throw creating_a_raster_failed{};
        }

        bool delete_files = (is_temp == is_temporary::yes);
        // Pass the original path to the deleter
        return std::shared_ptr<GDALRasterBand>(band, gdal_band_and_dataset_deleter(dataset, delete_files));
      }

      
      std::shared_ptr<GDALRasterBand> create_band_from_model(
        const std::filesystem::path& path
        , const gdal_raster_view_base& model,
        GDALDataType datatype, is_temporary is_temp)
      {
        int nBands = 1;
        GDALDataset* dataset = detail::create_standard_gdaldataset_from_model
        (path, model, datatype, nBands);

        if (dataset == nullptr) {
          throw creating_a_raster_failed{};
        }

        GDALRasterBand* band = dataset->GetRasterBand(1);
        if (band == nullptr) {
          GDALClose(dataset);
          throw creating_a_raster_failed{};
        }

        bool delete_files = (is_temp == is_temporary::yes);
        // Pass the original path to the deleter
        return std::shared_ptr<GDALRasterBand>(band, gdal_band_and_dataset_deleter(dataset, delete_files));
      }
     
      std::shared_ptr<GDALRasterBand> create_compressed_band_from_model(
        const std::filesystem::path& path
        , const gdal_raster_view_base& model,
        GDALDataType datatype, is_temporary is_temp)
      {
        int nBands = 1;
        GDALDataset* dataset = detail::create_compressed_gdaldataset_from_model
        (path, model, datatype, nBands);

        if (dataset == nullptr) {
          throw creating_a_raster_failed{};
        }

        GDALRasterBand* band = dataset->GetRasterBand(1);
        if (band == nullptr) {
          GDALClose(dataset);
          throw creating_a_raster_failed{};
        }

        bool delete_files = (is_temp == is_temporary::yes);
        // Pass the original path to the deleter
        return std::shared_ptr<GDALRasterBand>(band, gdal_band_and_dataset_deleter(dataset, delete_files));
      }
    } // detail
    /*
    any_blind_raster open_any(
      const std::filesystem::path& path,
      access elem_access,
      int band_index)
    {
      auto dataset = detail::open_dataset(path, elem_access);
      auto band = detail::open_band(dataset, band_index);

      switch (band->GetRasterDataType())
      {
      case GDT_Byte:     return make_any_blind_raster(gdal_raster_view<uint8_t>(band));
      case GDT_Int16:    return make_any_blind_raster(gdal_raster_view<int16_t>(band));
      case GDT_UInt16:   return make_any_blind_raster(gdal_raster_view<uint16_t>(band));
      case GDT_Int32:    return make_any_blind_raster(gdal_raster_view<int32_t>(band));
      case GDT_UInt32:   return make_any_blind_raster(gdal_raster_view<uint32_t>(band));
      case GDT_Float32:  return make_any_blind_raster(gdal_raster_view<float>(band));
      case GDT_Float64:  return make_any_blind_raster(gdal_raster_view<double>(band));
    // Complex values are now not supported
    //
      //case GDT_CInt16:   return make_any_blind_raster(gdal_raster_view<cint16_t>(band));
      //case GDT_CInt32:   return make_any_blind_raster(gdal_raster_view<cint32_t>(band));
      //case GDT_CFloat32: return make_any_blind_raster(gdal_raster_view<cfloat32_t>(band));
      //case GDT_CFloat64: return make_any_blind_raster(gdal_raster_view<cfloat64_t>(band));
      default:
        throw("could not open raster of unknown or complex type");
        return any_blind_raster{};
      }
    }

    export_any_helper::export_any_helper(const std::filesystem::path& path,
      const gdal_raster_view_base& model)
      : m_path(path), m_model(std::ref(model))
    {}

    export_any_helper::export_any_helper(const std::filesystem::path& path)
      : m_path(path), m_model()
    {}

    void export_any(const std::filesystem::path& path
      , any_blind_raster raster, const gdal_raster_view_base& model)
    {
      blind_function(export_any_helper{ path, model }, raster);
    }

    void export_any(const std::filesystem::path& path, any_blind_raster raster)
    {
      blind_function(export_any_helper{path}, raster);
    }
    */
  }
}
