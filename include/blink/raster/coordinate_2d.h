//
//=======================================================================
// Copyright 2015
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// The class coordinate_2d is used to provide 2d coordinates as an index to 
// rasters. 
// It could possibly be taken from an external source, e.g. BOOST

#ifndef BLINK_RASTER_COORDINATE_2D_H_AHZ
#define BLINK_RASTER_COORDINATE_2D_H_AHZ

#include <cstddef> //ptrdiff_t

namespace blink {
  namespace raster {

    class coordinate_2d
    {
    public:
      typedef std::ptrdiff_t index_type;
      typedef coordinate_2d this_type;

      // default constructor
      coordinate_2d(index_type r = 0, index_type c = 0) : row(r), col(c)
      {}

      this_type& operator+=(const coordinate_2d& that)
      {
        row += that.row;
        col += that.col;
        return *this;
      }

      this_type& operator-=(const coordinate_2d& that)
      {
        row -= that.row;
        col -= that.col;
        return *this;
      }

      template<class T>
      this_type& operator*=(const T& factor)
      {
        row *= factor;
        col *= factor;
        return *this;
      }

      template<class T>
      this_type& operator/=(const T& factor)
      {
        row /= factor;
        col /= factor;
        return *this;
      }

      this_type operator+(const coordinate_2d& that) const
      {
        this_type copy(*this);
        return  copy += that;
      }

      this_type operator-(const coordinate_2d& that) const
      {
        this_type copy(*this);
        return copy -= that;
      }

      template<class T>
      this_type operator*(const T& factor) const
      {
        return this_type(*this) *= factor;
      }

      template<class T>
      this_type operator/(const T& factor) const
      {
        return this_type(*this) /= factor;
      }

      bool operator==(const coordinate_2d& that) const
      {
        return row == that.row && col == that.col;
      }

      bool operator!=(const coordinate_2d& that) const
      {
        return row != that.row || col != that.col;
      }

      index_type row;
      index_type col;
    };
  }
} //namespace moving_window
#endif