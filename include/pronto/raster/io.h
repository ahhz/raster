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
#include <pronto/raster/any_blind_raster.h>
#include <pronto/raster/assign.h>
#include <pronto/raster/blind_function.h>
#include <pronto/raster/exceptions.h>
#include <pronto/raster/gdal_includes.h>
#include <pronto/raster/gdal_raster_view.h>
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

    template<class T, class IterationType = multi_pass>
    gdal_raster_view<T, IterationType> open(
      const std::filesystem::path& path, IterationType = IterationType{},
      access elem_access = access::read_write,
      int band_index = 1)
    {
      auto dataset = detail::open_dataset(path, elem_access);
      auto band = detail::open_band(dataset, band_index);
      return gdal_raster_view<T, IterationType>(band);
    }

    template<class T, class IterationType = multi_pass>
    gdal_raster_view<T, IterationType> create(
      const std::filesystem::path& path, int rows, int cols, IterationType = IterationType{}
      , GDALDataType data_type = detail::native_gdal_data_type<T>::value)
    {
      if (data_type == GDT_Unknown)
      {
        throw(creating_a_raster_failed{});
      }

      std::shared_ptr<GDALRasterBand> band = detail::create_band
        (path, rows, cols, data_type, is_temporary::no);

      return gdal_raster_view<T, IterationType>(band);
    }

    
    template<class T, class IterationType = multi_pass>
    gdal_raster_view<T, IterationType> create_temp(
      int rows, int cols, IterationType it = IterationType{},
      GDALDataType data_type = detail::native_gdal_data_type<T>::value)
    {
      if (data_type == GDT_Unknown) {
        throw(creating_a_raster_failed{});
      }

      auto path = detail::get_temp_tiff_path();

      std::shared_ptr<GDALRasterBand> band = detail::create_band
        (path, rows, cols, data_type, is_temporary::yes);

      return gdal_raster_view<T, IterationType>(band);
    }

    template<class T, class IterationType = multi_pass>
    gdal_raster_view<T, IterationType> create_from_model
      ( const std::filesystem::path& path
      , const gdal_raster_view_base& model, IterationType it = IterationType{}
      , GDALDataType data_type = detail::native_gdal_data_type<T>::value)
    {
      if (data_type == GDT_Unknown) {
        throw(creating_a_raster_failed{});
      }

      auto band = detail::create_band_from_model(path, model, data_type
        , is_temporary::no);

      return gdal_raster_view<T, IterationType>(band);
    }

    template<class T, class IterationType = multi_pass>
    gdal_raster_view<T, IterationType> create_compressed_from_model
    (const std::filesystem::path& path
      , const gdal_raster_view_base& model, IterationType = IterationType{}
      , GDALDataType data_type = detail::native_gdal_data_type<T>::value)
    {
      if (data_type == GDT_Unknown) {
        throw(creating_a_raster_failed{});
      }

      auto band = detail::create_compressed_band_from_model(path, model
        , data_type, is_temporary::no);

      return gdal_raster_view<T, IterationType>(band);
    }

    template<class T, class IterationType = multi_pass>
    gdal_raster_view<T, IterationType> create_temp_from_model(
      const gdal_raster_view_base& model
      , GDALDataType data_type = detail::native_gdal_data_type<T>::value)
    {
      if (data_type == GDT_Unknown) {
        throw(creating_a_raster_failed{});
      }

      auto path = detail::get_temp_tiff_path();

      auto band = detail::create_band_from_model(path, model, data_type
        , is_temporary::yes);

      return gdal_raster_view<T, IterationType>(band);
    }

    template<class T>
    gdal_raster_view<T> make_gdalrasterdata_view(
      std::shared_ptr<GDALRasterBand> band
      , GDALDataType data_type = detail::native_gdal_data_type<T>::value)
    {
      if (data_type == GDT_Unknown) {
        throw(creating_a_raster_failed{});
      }

      return gdal_raster_view<T>(band);
    }

    template<class T>
    gdal_raster_view<T> make_gdalrasterdata_view(
      GDALRasterBand* band
      , GDALDataType data_type = detail::native_gdal_data_type<T>::value)
    {
      auto sh_band(band, [](GDALRasterBand*) {});
      return make_gdalrasterdata_view<T>(sh_band, data_type);
    }

    any_blind_raster open_any(const std::filesystem::path& path
      , access elem_access = access::read_write, int band_index = 1);

    struct export_any_helper
    {
      export_any_helper(const std::filesystem::path& path, const gdal_raster_view_base& model);
      export_any_helper(const std::filesystem::path& path);

      template<class T>
      void operator()(any_raster<T> in) {
        if (m_model) {
          auto out = create_from_model<T>(m_path, *m_model);
          assign(out, in);
        } else {
          auto out = create<T>(m_path, in.rows(), in.cols());
          assign(out, in);
        }
      }

      const std::filesystem::path m_path;
      std::optional<std::reference_wrapper<const gdal_raster_view_base > > m_model;
    };

    void export_any(const std::filesystem::path& path, any_blind_raster raster
      , const gdal_raster_view_base& model);

    void export_any(const std::filesystem::path& path, any_blind_raster raster);
  }
}
