//
//=======================================================================
// Copyright 2015-2022
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
#include <pronto/raster/assign.h>
#include <pronto/raster/exceptions.h>
#include <pronto/raster/gdal_includes.h>
#include <pronto/raster/gdal_raster_view.h>
#include <pronto/raster/uncasted_gdal_raster_view.h>
#include <pronto/raster/optional.h>

#include <filesystem>
#include <iostream>

namespace pronto
{
  namespace raster
  {
    enum class is_temporary
    {
      no,
      yes
    };

    namespace detail {
      template<typename T> struct native_gdal_data_type
      {
        static const GDALDataType value = GDT_Unknown;
      };

      template<> struct native_gdal_data_type<int32_t>
      {
        static const GDALDataType value = GDT_Int32;
      };

      template<>
      struct native_gdal_data_type<uint32_t>
      {
        static const GDALDataType value = GDT_UInt32;
      };

      template<> struct native_gdal_data_type<int16_t>
      {
        static const GDALDataType value = GDT_Int16;
      };

      template<> struct native_gdal_data_type<uint16_t>
      {
        static const GDALDataType value = GDT_UInt16;
      };

      template<> struct native_gdal_data_type<uint8_t>
      {
        static const GDALDataType value = GDT_Byte;
      };

      template<> struct native_gdal_data_type<bool>
      {
        static const GDALDataType value = GDT_Byte;
      };

      template<> struct native_gdal_data_type<float>
      {
        static const GDALDataType value = GDT_Float32;
      };

      template<> struct native_gdal_data_type<double>
      {
        static const GDALDataType value = GDT_Float64;
      };
  /*
      template<> struct native_gdal_data_type<cint16_t>
      {
        static const GDALDataType value = GDT_CInt16;
      };

      template<> struct native_gdal_data_type<cint32_t>
      {
        static const GDALDataType value = GDT_CInt32;
      };
      template<> struct native_gdal_data_type<cfloat32_t>
      {
        static const GDALDataType value = GDT_CFloat32;
      };
      template<> struct native_gdal_data_type<cfloat64_t>
      {
        static const GDALDataType value = GDT_CFloat64;
      };

      */

  
      GDALDataset* create_compressed_gdaldataset(
          const std::filesystem::path& path, int rows, int cols
          , GDALDataType datatype, int nBands = 1);
      
      GDALDataset* create_standard_gdaldataset(
          const std::filesystem::path& path, int rows, int cols
          , GDALDataType datatype, int nBands = 1);
      
      GDALDataset* create_compressed_gdaldataset_from_model
      (const std::filesystem::path& path
          , const gdal_raster_view_base& model
          , GDALDataType datatype, int nBands = 1);
      
      GDALDataset* create_standard_gdaldataset_from_model
      (const std::filesystem::path& path
          , const gdal_raster_view_base& model
          , GDALDataType datatype, int nBands = 1);

      std::filesystem::path get_temp_tiff_path();
      void optionally_update_statistics(GDALRasterBand* band);

      struct dataset_deleter
      {
          void operator()(GDALRasterBand* band) const;
      };

      struct dataset_closer
      {
        dataset_closer(GDALDataset* dataset);
        void operator()(GDALRasterBand* band) const;
        GDALDataset* m_dataset;
      };

      std::shared_ptr<GDALDataset> open_dataset(
          const std::filesystem::path& path, access access);

      std::shared_ptr<GDALRasterBand> open_band(std::shared_ptr<GDALDataset> dataset, int band = 1);
      std::shared_ptr<GDALRasterBand> create_band(
          const std::filesystem::path& path, int rows, int cols,
          GDALDataType datatype, is_temporary is_temp);
      std::shared_ptr<GDALRasterBand> create_band_from_model(
          const std::filesystem::path& path
          , const gdal_raster_view_base& model,
          GDALDataType datatype, is_temporary is_temp);
      std::shared_ptr<GDALRasterBand> create_compressed_band_from_model(
          const std::filesystem::path& path
          , const gdal_raster_view_base& model,
          GDALDataType datatype, is_temporary is_temp);
    } // detail
    template<class T>
    static const GDALDataType gdal_data_type = detail::native_gdal_data_type<T>::value;

    template<class T, iteration_type IterationType = iteration_type::multi_pass, access AccessType = access::read_write>
    gdal_raster_view<T, IterationType, AccessType> open(const std::filesystem::path& path,int band_index = 1)
    {
      auto dataset = detail::open_dataset(path, AccessType);
      auto band = detail::open_band(dataset, band_index);
      return gdal_raster_view<T, IterationType, AccessType>(band);
    }

    template<class T, iteration_type IterationType = iteration_type::multi_pass>
    auto create(const std::filesystem::path& path, int rows, int cols, GDALDataType data_type = gdal_data_type<T>)
    {
      if (data_type == GDT_Unknown)
      {
        throw(creating_a_raster_failed{});
      }

      std::shared_ptr<GDALRasterBand> band = detail::create_band
        (path, rows, cols, data_type, is_temporary::no);

      return gdal_raster_view<T, IterationType, access::read_write>(band);
    }

    
    template<class T, iteration_type IterationType = iteration_type::multi_pass>
    auto create_temp(int rows, int cols, GDALDataType data_type = gdal_data_type<T>)
    {
      if (data_type == GDT_Unknown) {
        throw(creating_a_raster_failed{});
      }

      auto path = detail::get_temp_tiff_path();

      std::shared_ptr<GDALRasterBand> band = detail::create_band
        (path, rows, cols, data_type, is_temporary::yes);

      return gdal_raster_view<T, IterationType, access::read_write>(band);
    }

    // this is still an experiment
    template<class T, iteration_type IterationType = iteration_type::multi_pass>
    auto create_temp_uncasted(int rows, int cols)
    {
      auto data_type = gdal_data_type<T>;
      if (data_type == GDT_Unknown) {
        throw(creating_a_raster_failed{});
      }

      auto path = detail::get_temp_tiff_path();

      std::shared_ptr<GDALRasterBand> band = detail::create_band
      (path, rows, cols, data_type, is_temporary::yes);

      return uncasted_gdal_raster_view<T, IterationType, access::read_write>(band);
    }

    template<class T, iteration_type IterationType = iteration_type::multi_pass>
    auto create_from_model
      ( const std::filesystem::path& path
      , const gdal_raster_view_base& model
      , GDALDataType data_type = gdal_data_type<T>)
    {
      if (data_type == GDT_Unknown) {
        throw(creating_a_raster_failed{});
      }

      auto band = detail::create_band_from_model(path, model, data_type
        , is_temporary::no);

      return gdal_raster_view<T, IterationType, access::read_write>(band);
    }

    template<class T, iteration_type IterationType = iteration_type::multi_pass>
    auto create_compressed_from_model
    (const std::filesystem::path& path
      , const gdal_raster_view_base& model
      , GDALDataType data_type = gdal_data_type<T>)
    {
      if (data_type == GDT_Unknown) {
        throw(creating_a_raster_failed{});
      }

      auto band = detail::create_compressed_band_from_model(path, model
        , data_type, is_temporary::no);

      return gdal_raster_view<T, IterationType, access::read_write>(band);
    }

    template<class T, iteration_type IterationType = iteration_type::multi_pass>
    auto create_temp_from_model(
      const gdal_raster_view_base& model
      , GDALDataType data_type = gdal_data_type<T>)
    {
      if (data_type == GDT_Unknown) {
        throw(creating_a_raster_failed{});
      }

      auto path = detail::get_temp_tiff_path();

      auto band = detail::create_band_from_model(path, model, data_type
        , is_temporary::yes);

      return gdal_raster_view<T, IterationType, access::read_write>(band);
    }

    template<class T, iteration_type IterationType = iteration_type::multi_pass, access AccessType = access::read_write>
    auto make_gdalrasterdata_view(std::shared_ptr<GDALRasterBand> band, GDALDataType data_type = gdal_data_type<T>)
    {
      if (data_type == GDT_Unknown) {
        throw(creating_a_raster_failed{});
      }

      return gdal_raster_view<T,IterationType, AccessType>(band);
    }

    template<class T, iteration_type IterationType = iteration_type::multi_pass, access AccessType = access::read_write>
    auto make_gdalrasterdata_view(
      GDALRasterBand* band
      , GDALDataType data_type =gdal_data_type<T>)
    {
      auto sh_band(band, [](GDALRasterBand*) {});
      return make_gdalrasterdata_view<T, IterationType, AccessType>(sh_band, data_type);
    }

    template<class T, iteration_type I = iteration_type::multi_pass, access A = access::read_write>
    gdal_raster_variant<I, A> open_variant_typed(std::shared_ptr<GDALRasterBand> band)
    {
      auto raster = uncasted_gdal_raster_view <T, I, A>(band);
      auto nodata_value = raster.get_nodata_value();
      if (nodata_value) {
        return gdal_raster_variant<I, A>{nodata_to_optional(raster, *nodata_value)};
      }
      else {
        return gdal_raster_variant<I, A>{raster};
      }
    }


    template<class T, iteration_type I = iteration_type::multi_pass, access A = access::read_write>
    auto open_variant_typed(const std::filesystem::path& path,
      int band_index = 1)
    {
      auto dataset = detail::open_dataset(path, A);
      auto band = detail::open_band(dataset, band_index);
      return uncasted_gdal_raster_view <T, I, A>(band);
    }

    template<iteration_type IterationType = iteration_type::multi_pass, access AccessType = access::read_write>
    gdal_raster_variant<IterationType, AccessType> open_variant(const std::filesystem::path& path,
      int band_index = 1)
    {
      auto dataset = detail::open_dataset(path, AccessType);
      auto band = detail::open_band(dataset, band_index);
      switch (band->GetRasterDataType())
      {
      case GDT_Byte:     return open_variant_typed<uint8_t, IterationType, AccessType>(band);
      case GDT_Int16:    return open_variant_typed<int16_t, IterationType, AccessType>(band);
      case GDT_UInt16:   return open_variant_typed<uint16_t, IterationType, AccessType>(band);
      case GDT_Int32:    return open_variant_typed<int32_t, IterationType, AccessType>(band);
      case GDT_UInt32:   return open_variant_typed<uint32_t, IterationType, AccessType>(band);
      case GDT_Float32:  return open_variant_typed<float, IterationType, AccessType>(band);
      case GDT_Float64:  return open_variant_typed<double, IterationType, AccessType>(band);
        // Complex values are now not supported
        //
          //case GDT_CInt16:   return 
          //case GDT_CInt32:   return 
          //case GDT_CFloat32: return
          //case GDT_CFloat64: return 
      }
      throw("could not open raster of unknown or complex type");
      return  gdal_raster_variant<IterationType, AccessType>{};
    }
  }
}
