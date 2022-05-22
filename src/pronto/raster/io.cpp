#include <pronto/raster/io.h>

namespace pronto {
	namespace raster {
		namespace detail {
  GDALDataset* create_compressed_gdaldataset(
	const filesystem::path& path, int rows, int cols
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
	const filesystem::path& path, int rows, int cols
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
  (const filesystem::path& path
	, const gdal_raster_view_base& model
	, GDALDataType datatype, int nBands )
  {
	//int rows = const_cast<GDALDataset*>(model)->GetRasterYSize();
	//int cols = const_cast<GDALDataset*>(model)->GetRasterXSize();

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
	( const filesystem::path& path
	, const gdal_raster_view_base& model
	, GDALDataType datatype, int nBands )
  {
	//int rows = const_cast<GDALDataset*>(model)->GetRasterYSize();
	//int cols = const_cast<GDALDataset*>(model)->GetRasterXSize();

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

  filesystem::path get_temp_tiff_path()
  {
	auto temp_path = filesystem::temp_directory_path();
	filesystem::path unique_temp_path_model = temp_path /= "%%%%-%%%%-%%%%-%%%%.tif";
	return get_unique_path(unique_temp_path_model);
  }


void dataset_deleter::operator()(GDALRasterBand* band) const
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
 
inline void optionally_update_statistics(GDALRasterBand* band)
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

dataset_closer::dataset_closer(GDALDataset* dataset) : m_dataset(dataset)
{}

void dataset_closer::operator()(GDALRasterBand* band) const
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
std::shared_ptr<GDALDataset> open_dataset(
	const filesystem::path& path, access access)
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

	std::shared_ptr<GDALRasterBand> create_band(
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

  std::shared_ptr<GDALRasterBand> create_band_from_model(
	const filesystem::path& path
	, const gdal_raster_view_base& model,
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
 std::shared_ptr<GDALRasterBand> create_compressed_band_from_model(
	const filesystem::path& path
	, const gdal_raster_view_base& model,
	GDALDataType datatype, is_temporary is_temp)
  {
	int nBands = 1;
	GDALDataset* dataset = detail::create_compressed_gdaldataset_from_model
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

  any_blind_raster open_any(
      const filesystem::path& path,
      access elem_access,
      int band_index)
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

    inline void export_any(const filesystem::path& path, any_blind_raster raster)
    {
      blind_function(export_any_helper<int>{path, none}, raster);
    }

  }
}
