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
#include <pronto/raster/gdal_block.h>
#include <pronto/raster/gdal_includes.h>
#include <pronto/raster/gdal_raster_iterator.h>

#include <memory> //shared_ptr
#include <ranges>
#include <optional>

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
      static const bool is_single_pass = IterationType == iteration_type::single_pass;
      static const bool is_mutable = AccessType != access::read_only;

    public:
      using block_type = block<T, IterationType, AccessType>;

      gdal_raster_view(std::shared_ptr<GDALRasterBand> band)
      {
        if (!band) throw(gdal_raster_view_works_on_unitialized_band{});
        m_band = band;
        m_rows = band->GetYSize();
        m_cols = band->GetXSize();
        m_first_row = 0;
        m_first_col = 0;
      }
       
      gdal_raster_view() : m_band(nullptr), m_rows(0), m_cols(0), m_first_row(0), m_first_col(0)
      {
      }
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
      
      CPLErr get_geo_transform(double* padfTransform) const
      {
        if (!m_band) throw(gdal_raster_view_works_on_unitialized_band{});
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
        if (!m_band) throw(gdal_raster_view_works_on_unitialized_band{});
        int check = 0;
        double value = m_band->GetNoDataValue(&check);
        if (check) return static_cast<T>(value);
        return std::nullopt;
      }

      void set_nodata_value(bool has_nodata, const T& value) const
      {
        if (!m_band) throw(gdal_raster_view_works_on_unitialized_band{});
        CPLErr err = has_nodata ? m_band->SetNoDataValue(value) : m_band->DeleteNoDataValue();
        if (err != CE_None) throw(gdal_raster_view_set_nodata_value_failed{});
      }

      int get_block_rows() const
      {
        if (!m_band) throw(gdal_raster_view_works_on_unitialized_band{});
        int rows, cols;
        m_band->GetBlockSize(&cols, &rows);
        return rows;
      }
      
      int get_block_cols() const
      {
        if (!m_band) throw(gdal_raster_view_works_on_unitialized_band{});
        int rows, cols;
        m_band->GetBlockSize(&cols, &rows);
        return cols;
      }

      void reset_block(block_type& block, int block_row, int block_col) const 
      {
        if (!m_band) throw(gdal_raster_view_works_on_unitialized_band{});

        block.reset(m_band.get(), block_row, block_col);
        if constexpr (is_mutable) {
          if(m_band->GetAccess() == GA_Update) {
             block.mark_dirty();
            }
          }
      }

    private:
      friend class gdal_raster_iterator<value_type, IterationType, AccessType>;
      std::shared_ptr<GDALRasterBand> m_band;
      int m_rows;
      int m_cols;
      int m_first_row;
      int m_first_col;
    };
  }
}
