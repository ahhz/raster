//=======================================================================
// Copyright 2015
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// This header file provides a wrapper around a GDALRasterBand that allows it
// to be used by the gdal_raster_iterator (which iterates row-by-row or
// column-by-column) over the rasterdata.
//

#pragma once

#include <pronto/raster/access_type.h>
#include <pronto/raster/exceptions.h>
#include <pronto/raster/gdal_includes.h>
#include <pronto/raster/gdal_raster_iterator.h>
#include <pronto/raster/optional.h>

#include <cassert>
#include <cstdint>
#include <memory>
#include <ranges>
#include <type_traits>

namespace pronto
{
  namespace raster
  {
      class gdal_raster_view_base
      {
      public:
          virtual int rows() const = 0;
          virtual int cols() const = 0;
          virtual int size() const = 0;
          virtual CPLErr get_geo_transform(double* padfTransform) const = 0;
          virtual std::shared_ptr<GDALRasterBand> get_band() const = 0;
      };


    template<class T, iteration_type IterationType = iteration_type::multi_pass, access AccessType= access::read_write>
    class gdal_raster_view : public std::ranges::view_interface<gdal_raster_view<T, IterationType> >, public gdal_raster_view_base
    {
    private:
      using value_type = T;

    public:
      gdal_raster_view(std::shared_ptr<GDALRasterBand> band)
        : m_band(band), m_rows(band->GetYSize()), m_cols(band->GetXSize()),m_first_row(0), m_first_col(0), put(nullptr), get(nullptr)
      {
        GDALDataType datatype = m_band->GetRasterDataType();
        m_stride = GDALGetDataTypeSize(datatype) / 8;

        // Using pointers to member functions as a means of run-time polymorphism
        static_assert(sizeof(float) == 4, "GDAL assumes size of float is 4 bytes");
        static_assert(sizeof(double) == 8, "GDAL assumes size of double is 8 bytes");
        switch (m_band->GetRasterDataType())
        {
        case GDT_Byte:     set_accessors<uint8_t >();   break;
        case GDT_Int16:    set_accessors<int16_t >();   break;
        case GDT_UInt16:   set_accessors<uint16_t>();   break;
        case GDT_Int32:    set_accessors<int32_t>();    break;
        case GDT_UInt32:   set_accessors<uint32_t>();   break;
        case GDT_Float32:  set_accessors<float>();      break;
        case GDT_Float64:  set_accessors<double>();     break;
        // Complex numbers not currently supported
		    //
		    //case GDT_CInt16:   set_accessors<cint16_t>();   break;
        //case GDT_CInt32:   set_accessors<cint32_t>();   break;
        //case GDT_CFloat32: set_accessors<cfloat32_t>(); break;
        //case GDT_CFloat64: set_accessors<cfloat64_t>(); break;
        default: break;
        }
        if (m_band->GetAccess() == GA_ReadOnly) {
          put = gdal_raster_view::put_nothing;
        }
      }
      gdal_raster_view() = default;

      // using the aliasing constructor seems overly complicated now. Just remove for a
      // deleter that does nothing?
      gdal_raster_view(GDALRasterBand* band)
        : gdal_raster_view(std::shared_ptr<GDALRasterBand>{
        std::shared_ptr<GDALRasterBand>{}, band})
      {};

      using iterator = gdal_raster_iterator<value_type, IterationType, AccessType>;
     
      std::shared_ptr<GDALRasterBand> get_band() const
      {
        return m_band;
      }
      //  friend create_standard_gdaldataset_from_model

      CPLErr get_geo_transform(double* padfTransform) const
      {
        CPLErr err = m_band->GetDataset()->GetGeoTransform(padfTransform);

        // Set this default affine transformation to be consistent with ARCGIS
        // For a raster with missing transformation, arcgis centers the top
        // left cell at (0,0). The cell-size is 1, the positive y-directions
        // direction is South to North.
        //
        if (err == CE_Failure) {
          padfTransform[0] = -0.5;
          padfTransform[1] = 1;
          padfTransform[2] = 0;
          padfTransform[3] = 0.5;
          padfTransform[4] = 0;
          padfTransform[5] = -1;
        }

        // Modify the transform such that is only for the sub-raster and not for
        // the dataset.
        padfTransform[0] = padfTransform[0]
          + padfTransform[1] * m_first_col
          + padfTransform[2] * m_first_row;

        padfTransform[3] = padfTransform[3]
          + padfTransform[4] * m_first_col
          + padfTransform[5] * m_first_row;

        return err;
      }

      int rows() const
      {
        return m_rows;
      };

      int cols() const
      {
        return m_cols;
      };

      int size() const
      {
        return rows() * cols();
      };

      gdal_raster_view sub_raster(int first_row, int first_col, int rows, int cols) const
      {
        gdal_raster_view out{m_band};
        out.m_first_row = m_first_row + first_row;
        out.m_first_col = m_first_col + first_col;
        out.m_rows = rows;
        out.m_cols = cols;
        return out;
      }

      iterator begin() const
      {
        iterator i;
        i.find_begin(this);
        return i;
      }

      iterator end() const
      {
        iterator i;
        i.find_end(this);
        return i;
      }
      
      std::optional<T> get_nodata_value() const
      {
        int* check = nullptr;
        double value = m_band->GetNoDataValue(check);
        if (check) return std::optional<T>{static_cast<T>(value)};
        return std::optional<T>{};
      }

      void set_nodata_value(bool has_nodata, const T& value) const
      {
        if (has_nodata) m_band->SetNoDataValue(value);
        else m_band->DeleteNoDataValue();
      }

    private:
      //friend class iterator;
      //friend class const_iterator;
      friend class gdal_raster_iterator<value_type, IterationType, AccessType>;
   
      template<typename U>
      static void put_special(const value_type& value, void* const target)
      {
        *(static_cast<U*>(target)) = static_cast<U>(value);
      }

      static void put_nothing(const value_type& value, void* const target)
      {
          assert(false);
          throw(writing_to_raster_failed{});
      }

      template<typename U>
      static value_type get_special(const void* const source)
      {
        return static_cast<value_type>(*static_cast<const U*>(source));
      }
	  
      template<typename U> void set_accessors()
      {
        put = gdal_raster_view::put_special<U>;
        get = gdal_raster_view::get_special<U>;
      }

      // function pointers for "runtime polymorphism" based on file datatype.
      void(*put)(const value_type&, void* const);
      value_type(*get)(const void* const);

      // Using std::function is just a bit less efficient
      //std::function<void(const value_type&, void* const)> put;
      //std::function<value_type(const void* const)> get;

      unsigned char m_stride;

      std::shared_ptr<GDALRasterBand> m_band;
      int m_rows;
      int m_cols;
      int m_first_row;
      int m_first_col;
    };
  }
}
