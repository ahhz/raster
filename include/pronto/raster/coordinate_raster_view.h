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
#include <pronto/raster/coordinate_raster_iterator.h>
#include <utility>

namespace pronto
{
  namespace raster
  {
    template<class CoordinatePair>
    class coordinate_raster_view
    {
    private:
      using value_type = CoordinatePair;

    public:
      template<class OtherRaster>
      coordinate_raster_view(const OtherRaster& r) // also serves as copy constructor
        : m_rows(r.rows()), m_cols(r.cols()),m_first_row(0), m_first_col(0)
      {
      }
      coordinate_raster_view(int rows, int cols) 
        : m_rows(rows), m_cols(cols), m_first_row(0), m_first_col(0)
      {}

      coordinate_raster_view() = default;
        
      using iterator = coordinate_raster_iterator<value_type>;
      using const_iterator = iterator;

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

      coordinate_raster_view sub_raster(int first_row, int first_col, int rows, int cols) const
      {
        coordinate_raster_view out{};
        out.m_first_row = m_first_row + first_row;
        out.m_first_col = m_first_col + first_col;
        out.m_rows = rows;
        out.m_cols = cols;
        return out;
      }

      iterator begin()
      {
        iterator i;
        i.find_begin(this);
        return i;
      }

      iterator end()
      {
        iterator i;
        i.find_end(this);
        return i;
      }

      const_iterator begin() const
      {
        const_iterator i;
        i.find_begin(this);
        return i;
      }

      const_iterator end() const
      {
        const_iterator i;
        i.find_end(this);
        return i;
      }

    private:
      friend class coordinate_raster_iterator<value_type>;
      int m_rows;
      int m_cols;
      int m_first_row;
      int m_first_col;
    };

    template<class OtherRaster, class CoordinatePair = std::pair<int, int>>
    coordinate_raster_view<CoordinatePair> coordinate_raster(const OtherRaster& r)
    {
      return coordinate_raster_view<CoordinatePair>(r);
    }

    template<class CoordinatePair = std::pair<int, int>, class T = typename CoordinatePair::first_type>
    coordinate_raster_view<CoordinatePair> coordinate_raster(T rows, T cols)
    {
      return coordinate_raster_view<CoordinatePair>(rows, cols);
    }
  }
}
