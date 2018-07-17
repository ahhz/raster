//
//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================

// TODO: When a GDALRasterBand is closed, now its parent GDALDataSet is 
// closed. This will be problematic when multiple bands from the same 
// dataset are used.  So for now we only support one band per 
// dataset.  

#pragma once
#include <pronto/raster/any_blind_raster.h>
#include <pronto/raster/assign.h>
#include <pronto/raster/blind_function.h>
#include <pronto/raster/exceptions.h>
#include <pronto/raster/filesystem.h>
#include <pronto/raster/gdal_data_types.h>
#include <pronto/raster/gdal_includes.h>
#include <pronto/raster/gdal_raster_view.h>
#include <pronto/raster/self_cached_gdal_raster_view.h>
#include <pronto/raster/optional.h>

#include <iostream>

namespace pronto
{
  namespace raster
  {
    enum is_temporary
    {
      yes, 
      no
    };

    namespace detail {
      static GDALDataset* create_standard_gdaldataset(
        const filesystem::path& path, int rows, int cols
        , GDALDataType datatype, int nBands = 1)
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

        GDALDataset* dataset = driver->Create(path.string().c_str(), cols, rows
          , nBands, datatype, papszOptions);
        return dataset;
      }

      template<class T>
      GDALDataset* create_standard_gdaldataset_from_model
        ( const filesystem::path& path
        , const gdal_raster_view<T>& model
        , GDALDataType datatype, int nBands = 1)
      {
        //int rows = const_cast<GDALDataset*>(model)->GetRasterYSize();
        //int cols = const_cast<GDALDataset*>(model)->GetRasterXSize();

        int rows = model.rows();
        int cols = model.cols();

        GDALDataset* model_data_set = model.get_band()->GetDataset();
        GDALDataset* dataset = create_standard_gdaldataset(path, rows, cols
          , datatype, nBands);

        if (dataset == NULL) return NULL;

        double gt_data[6];
        double* geotransform = gt_data;
        CPLErr error_status = model.get_geo_transform(geotransform);
        dataset->SetGeoTransform(geotransform);
        dataset->SetProjection(model_data_set->GetProjectionRef());
        return dataset;
      }

      static filesystem::path get_temp_tiff_path()
      {
        auto temp_path = filesystem::temp_directory_path();
        auto unique_temp_path_model = temp_path /= "%%%%-%%%%-%%%%-%%%%.tif";
        return get_unique_path(unique_temp_path_model);
      }


      struct dataset_deleter
      {
        void operator()(GDALRasterBand* band) const
        {
          GDALDataset* dataset = band ? band->GetDataset() : nullptr;
          if (dataset) {
            char** file_list = dataset->GetFileList();
            GDALDriver* driver = dataset->GetDriver();
            GDALClose(dataset);
            driver->Delete(file_list[0]);
            CSLDestroy(file_list);
          }
          else {
            std::cout << "Could not delete raster" << std::endl;
            throw(deleting_raster_failed{});
          }
        }
      };

      inline static void optionally_update_statistics(GDALRasterBand* band)
      {
        if (band && band->GetAccess() == GA_Update) {
          //  std::cout << " GA_Update " << std::endl;

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

      struct dataset_closer
      {
        dataset_closer(GDALDataset* dataset) : m_dataset(dataset)
        {}

        void operator()(GDALRasterBand* band) const
        {
          GDALDataset* dataset = band ? band->GetDataset() : nullptr;
          assert(dataset == m_dataset);
          if (dataset) {
            optionally_update_statistics(band);
            GDALClose(dataset);
          }
          else {
            throw(closing_raster_failed{});
          }

        }
        GDALDataset* m_dataset;

      };
      static std::shared_ptr<GDALDataset> open_dataset(
        const filesystem::path& path, access access)
      {
        GDALAllRegister();
        GDALDataset* dataset = (GDALDataset*)GDALOpen(path.string().c_str()
          , gdal_access(access));
        
        bool test = dataset->GetAccess() == gdal_access(access);
        if (!test)
        {
          std::cout << "GDALAccess mismatch: " << path.c_str() << std::endl;
          throw(opening_raster_failed{});
        }
        assert(test);
        if (dataset == nullptr) {
          std::cout << "Could not read: " << path.c_str() << std::endl;
          throw(opening_raster_failed{});
        }
        
        auto closer = [](GDALDataset* ds) {
          if(ds) GDALClose(ds);
          ds = nullptr;
        };

        return std::shared_ptr<GDALDataset>(dataset, closer);
      }

      static std::shared_ptr<GDALRasterBand> open_band(std::shared_ptr<GDALDataset> dataset, int band = 1)
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

      static  std::shared_ptr<GDALRasterBand> create_band(
        const filesystem::path& path, int rows, int cols,
        GDALDataType datatype, is_temporary is_temp)
      {
        int nBands = 1;
        GDALDataset* dataset = detail::create_standard_gdaldataset(path, rows
          , cols, datatype, nBands);
        if (dataset == nullptr)
        {
          std::cout << "Could not create raster file: " << path << std::endl;
          throw(creating_a_raster_failed{});
        }
        GDALRasterBand* band = dataset->GetRasterBand(1);

        return is_temp == is_temporary::yes
          ? std::shared_ptr<GDALRasterBand>(band, dataset_deleter{})
          : std::shared_ptr<GDALRasterBand>(band, dataset_closer(dataset));
      }

      template<class T>
      static std::shared_ptr<GDALRasterBand> create_band_from_model(
        const filesystem::path& path, 
        const gdal_raster_view<T>& model,
        GDALDataType datatype, is_temporary is_temp)
      {
        int nBands = 1;
        GDALDataset* dataset = detail::create_standard_gdaldataset_from_model
        (path, model, datatype, nBands);

        if (dataset == nullptr) {
          std::cout << "Could not create raster file: " << path << std::endl;
          throw(creating_a_raster_failed{});
        }
        GDALRasterBand* band = dataset->GetRasterBand(1);
        return is_temp == is_temporary::yes
          ? std::shared_ptr<GDALRasterBand>(band, dataset_deleter{})
          : std::shared_ptr<GDALRasterBand>(band, dataset_closer(dataset));
      }
    } // detail

    template<class T>
    gdal_raster_view<T> open(
      const filesystem::path& path,
      access elem_access = read_write,
      int band_index = 1)
    {
      auto dataset = detail::open_dataset(path, elem_access);
      auto band = detail::open_band(dataset, band_index);
      return gdal_raster_view<T>(band);
    }


    template<class T>
    gdal_raster_view<T, forward_only_iteration> open_forward_only(
      const filesystem::path& path,
      access elem_access = read_write,
      int band_index = 1)
    {
      auto dataset = detail::open_dataset(path, elem_access);
      auto band = detail::open_band(dataset, band_index);
      return gdal_raster_view<T, forward_only_iteration>(band);
    }

    template<class T>
    gdal_raster_view<T> create(
      const filesystem::path& path, int rows, int cols
      , GDALDataType data_type = detail::native_gdal_data_type<T>::value)
    {
      if (data_type == GDT_Unknown)
      {
        throw(creating_a_raster_failed{});
      }

      std::shared_ptr<GDALRasterBand> band = detail::create_band
        (path, rows, cols, data_type, is_temporary::no);

      return gdal_raster_view<T>(band);
    }

    template<class T>
    gdal_raster_view<T, forward_only_iteration> create_forward_only(
      const filesystem::path& path, int rows, int cols
      , GDALDataType data_type = detail::native_gdal_data_type<T>::value)
    {
      if (data_type == GDT_Unknown)
      {
        throw(creating_a_raster_failed{});
      }

      std::shared_ptr<GDALRasterBand> band = detail::create_band
      (path, rows, cols, data_type, is_temporary::no);

      return gdal_raster_view<T, forward_only_iteration>(band);
    }

    template<class T>
    gdal_raster_view<T> create_temp(
      int rows, int cols, 
      GDALDataType data_type = detail::native_gdal_data_type<T>::value)
    {
      if (data_type == GDT_Unknown)
      {
        throw(creating_a_raster_failed{});
      }

      auto path = detail::get_temp_tiff_path();

      std::shared_ptr<GDALRasterBand> band = detail::create_band
        (path, rows, cols, data_type, is_temporary::yes);

      return gdal_raster_view<T>(band);
    }

    template<class T, class U>
    gdal_raster_view<T> create_from_model
      ( const filesystem::path& path
      , const gdal_raster_view<U>& model
      , GDALDataType data_type = detail::native_gdal_data_type<T>::value)
    {
      if (data_type == GDT_Unknown)
      {
        throw(creating_a_raster_failed{});
      }

      std::shared_ptr<GDALRasterBand> band = detail::create_band_from_model
       (path, model, data_type, is_temporary::no);

      return gdal_raster_view<T>(band);
    }

    template<class T, class U>
    gdal_raster_view<T> create_temp_from_model(
      const gdal_raster_view<U>& model
      , GDALDataType data_type = detail::native_gdal_data_type<T>::value)
    {
      if (data_type == GDT_Unknown)
      {
        throw(creating_a_raster_failed{});
      }

      auto path = detail::get_temp_tiff_path();

      std::shared_ptr<GDALRasterBand> band = detail::create_band_from_model
        (path, model, data_type, is_temporary::yes);

      return gdal_raster_view<T>(band);
    }

    template<class T>
    gdal_raster_view<T> make_gdalrasterdata_view(
      std::shared_ptr<GDALRasterBand> band
      , GDALDataType data_type = detail::native_gdal_data_type<T>::value)
    {
      if (data_type == GDT_Unknown)
      {
        throw(creating_a_raster_failed{});
      }
      return gdal_raster_view<T>(band);
    }

    template<class T>
    gdal_raster_view<T> make_gdalrasterdata_view(
      GDALRasterBand* band
      , GDALDataType data_type = detail::native_gdal_data_type<T>::value)
    {
      std::shared_ptr<GDALRasterBand> sh_band(band, [](GDALRasterBand*) {});
      return make_gdalrasterdata_view<T>(sh_band, data_type);
    }

	// inline because this is a non-template function and strictly speaking 
	// this should be in a cpp file
    inline any_blind_raster open_any(
      const filesystem::path& path,
      access elem_access = read_write,
      int band_index = 1)
    {
      auto dataset = detail::open_dataset(path, elem_access);
      auto band = detail::open_band(dataset, band_index);
//      std::shared_ptr<GDALRasterBand> band = detail::open_band(path
//        , gdal_access(elem_access)
//       , band_index);

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

    template<class U>
    struct export_any_helper 
    {
      export_any_helper(const filesystem::path& path,
        optional<gdal_raster_view<U>> model)
        : m_path(path), m_model(model)
      {}

      template<class T>
      void operator()(any_raster<T> in)
      {
        if (m_model) {
          auto out = create_from_model<T>(m_path, *m_model);
          assign(out, in);
        }
        else
        {
          auto out = create<T>(m_path, in.rows(), in.cols());
          assign(out, in);
        }
      }
      
      const filesystem::path m_path;
      optional<gdal_raster_view<U> > m_model;
    };
    

    template<class U>
    void export_any(const filesystem::path& path
      , any_blind_raster raster, gdal_raster_view<U> model)
    {
      blind_function(export_any_helper<U>{path, model}, raster);
    }

    inline void export_any(const filesystem::path& path, any_blind_raster raster)
    {
      blind_function(export_any_helper<int>{path, none}, raster);
    }

  }
}
