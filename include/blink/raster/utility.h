// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// Contains the utility functions for reading and creating gdal_raster 
// TODO rename this file to something more specific

#ifndef BLINK_RASTER_UTILITY_H_AHZ
#define BLINK_RASTER_UTILITY_H_AHZ

#include <blink/raster/gdal_raster.h>

#include <gdal.h>
#include <gdal_priv.h>
#include <boost/filesystem.hpp>
#include <iostream>

namespace blink {
  namespace raster {


    template<typename T> struct native_gdal_data_type
    {
      static const GDALDataType type = GDT_Unknown;
    };

    template<> struct native_gdal_data_type<int>
    {
      static const GDALDataType type = GDT_Int32;
    };

    template<> 
    struct native_gdal_data_type<unsigned int>
    {
      static const GDALDataType type = GDT_UInt32;
    };

    template<> struct native_gdal_data_type<short>
    {
      static const GDALDataType type = GDT_Int16;
    };

    template<> struct native_gdal_data_type<unsigned short>
    {
      static const GDALDataType type = GDT_UInt16;
    };

    template<> struct native_gdal_data_type<unsigned char>
    {
      static const GDALDataType type = GDT_Byte;
    };

    template<> struct native_gdal_data_type<float>
    {
      static const GDALDataType type = GDT_Float32;
    };

    template<> struct native_gdal_data_type<double>
    {
      static const GDALDataType type = GDT_Float64;
    };

   namespace detail {

      GDALDataset* create_standard_gdaldataset(
        const boost::filesystem::path& path, int rows, int cols
        , GDALDataType datatype, int nBands = 1)
      {
        GDALAllRegister();

        GDALDriverManager* m = GetGDALDriverManager();
        GDALDriver* driver = m->GetDriverByName("GTiff");

        char **papszOptions = NULL;
        papszOptions = CSLSetNameValue(papszOptions, "BLOCKXSIZE", "256");
        papszOptions = CSLSetNameValue(papszOptions, "BLOCKYSIZE", "256");
        papszOptions = CSLSetNameValue(papszOptions, "TILED", "YES");
        papszOptions = CSLSetNameValue(papszOptions, "COMPRESS", "DEFLATE");

        GDALDataset* dataset = driver->Create(path.string().c_str(), cols, rows
          , nBands, datatype, papszOptions);
        return dataset;
      }

      GDALDataset* create_standard_gdaldataset_from_model(
        const boost::filesystem::path& path, const GDALDataset* model
        , GDALDataType datatype, int nBands = 1)
      {
        int rows = const_cast<GDALDataset*>(model)->GetRasterYSize();
        int cols = const_cast<GDALDataset*>(model)->GetRasterXSize();

        GDALDataset* dataset = create_standard_gdaldataset(path, rows, cols
          , datatype, nBands);

        if (dataset == NULL) return NULL;

        double gt_data[6];
        double* geotransform = gt_data;
        CPLErr error_status 
          = const_cast<GDALDataset*>(model)->GetGeoTransform(geotransform);
        dataset->SetGeoTransform(geotransform);
        dataset->SetProjection(
          const_cast<GDALDataset*>(model)->GetProjectionRef());
        return dataset;
      }

      boost::filesystem::path get_temp_tiff_path()
      {
        boost::filesystem::path temp_path 
          = boost::filesystem::temp_directory_path();
        boost::filesystem::path unique_temp_path_model 
          = temp_path /= "%%%%-%%%%-%%%%-%%%%.tif";
        return unique_path(unique_temp_path_model);
      }

      struct gdal_makers
      {
        template<typename T>
        static gdal_raster<T> open_gdal_raster(const boost::filesystem::path& path
          , GDALAccess access, int band)
        {
          return gdal_raster<T>(path, access, band);
        }

        template<typename T>
        static gdal_raster<T> create_gdal_raster(const boost::filesystem::path& path
          , int rows, int cols, GDALDataType datatype = native_gdal_data_type<T>::type)
        {
          int nBands = 1;
          GDALDataset* dataset = detail::create_standard_gdaldataset(path, rows
            , cols, datatype, nBands);
          if (dataset == nullptr)
          {
            std::cout << "Could not create raster file: " << path << std::endl;
            BOOST_THROW_EXCEPTION(creating_a_raster_failed{});
          }
          gdal_raster<T> raster(dataset, path, nBands);
          return raster;
        }

        template<typename T, typename U>
        static gdal_raster<T> create_gdal_raster_from_model(
          const boost::filesystem::path& path, const gdal_raster<U>& model, GDALDataType datatype = native_gdal_data_type<T>::type)
        {
          int nBands = 1;
          GDALDataset* dataset = detail::create_standard_gdaldataset_from_model(path, model.get_gdal_dataset(), datatype, nBands);
          if (dataset == nullptr)
          {
            std::cout << "Could not create raster file: " << path << std::endl;
            BOOST_THROW_EXCEPTION(creating_a_raster_failed{});
          }
          gdal_raster<T> raster(dataset, path, nBands);
          return raster;
        }

        template<typename T>
        static gdal_raster<T> create_temp_gdal_raster(int rows, int cols, GDALDataType datatype = native_gdal_data_type<T>::type)
        {
          boost::filesystem::path path = detail::get_temp_tiff_path();
          gdal_raster<T> raster = create_gdal_raster<T>(path, rows, cols, datatype);
          raster.set_delete_on_close(true);
          return raster;
        }

        template<typename T, typename U>
        static gdal_raster<T> create_temp_gdal_raster_from_model(const gdal_raster<U>& model, GDALDataType datatype = native_gdal_data_type<T>::type)
        {
          boost::filesystem::path path = get_temp_tiff_path();
          gdal_raster<T> raster = create_gdal_raster_from_model<T>(path, model, datatype);
          raster.set_delete_on_close(true);
          return raster;
        }
      };
    }
  
    template<typename T>
    gdal_raster<T> open_gdal_raster(const boost::filesystem::path& path
      , GDALAccess access, int band = 1)
    {
      return detail::gdal_makers::open_gdal_raster<T>(path, access, band);
    }

    template<typename T>
    gdal_raster<T> create_gdal_raster(const boost::filesystem::path& path
      , int rows, int cols, GDALDataType datatype = native_gdal_data_type<T>::type)
    {
      return detail::gdal_makers::create_gdal_raster<T>(path, rows, cols, datatype);
    }

    template<typename T, typename U>
    gdal_raster<T> create_gdal_raster_from_model(
      const boost::filesystem::path& path, const gdal_raster<U>& model, 
      GDALDataType datatype = native_gdal_data_type<T>::type)
    {
      return detail::gdal_makers::create_gdal_raster_from_model<T>(path, model,
        datatype);
    }

    template<typename T>
    gdal_raster<T> create_temp_gdal_raster(int rows, int cols, GDALDataType datatype = native_gdal_data_type<T>::type)
    {
      return detail::gdal_makers::create_temp_gdal_raster<T>(rows, cols, datatype);
    }

    template<typename T, typename U>
    gdal_raster<T> create_temp_gdal_raster_from_model(const gdal_raster<U>& model, GDALDataType datatype = native_gdal_data_type<T>::type)
    {
      return detail::gdal_makers::create_temp_gdal_raster_from_model<T>(model,
        datatype);
    }
  }
}
#endif