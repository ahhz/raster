//
//=======================================================================
// Copyright 2016-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once

#include <blink/raster/any_blind_raster.h>
#include <blink/raster/gdal_raster_view.h>

#include <blink/raster/pair_raster_view.h>

namespace blink {
  namespace raster {
      
    template<class RasterTo, class RasterFrom> // only needs to be a range
    void assign(RasterTo& to, const RasterFrom& from)
    {
      /* this works for rasters, but code below makes it more general*/
      //auto both = raster_pair(to, from);
      //for (auto&& v : both)
      //{
      // v.first = v.second;
      //}

      auto a = to.begin();
      auto b = from.begin();
      auto b_end = from.end();
      for (; b != b_end; ++b, ++a)
      {
       *a = *b;
      }
    }

	
    template<class FromRaster>
    struct blind_assign_to
    {
      blind_assign_to(FromRaster from) : m_from(from)
      {}
      
      template<class T>
      void operator()(any_raster<T> to) const
      {
        assign(to, m_from);
      }
    
    private: 
      FromRaster m_from;
    };

    template<class ToRaster>
    struct blind_assign_from
    {
      blind_assign_from(ToRaster to) : m_to(to)
      {}

      template<class T>
      void operator()(any_raster<T> from) const
      {
        assign(from, m_to);
      }
    private:
      ToRaster m_to;
    };

    template< class RasterFrom>
    void assign(any_blind_raster& to, const RasterFrom& from)
    {
      blind_function(blind_assign_to<RasterFrom>(from), to);
    }

    template< class RasterTo>
    void assign(RasterTo& to, const any_blind_raster& from)
    {
      blind_function(blind_assign_from<RasterTo>(to), from);
    }

    
    struct blind_assign_from_to
    {
      blind_assign_from_to(any_blind_raster to) : m_to(to)
      {

      }
      template<class T>
      void operator()(any_raster<T> from) const
      {
        assign(from, m_to);
      }

      any_blind_raster m_to;
    };

    void assign(any_blind_raster& to, const any_blind_raster& from)
    {
      blind_function(blind_assign_from_to{to}, from);
    }
    
    template<class RasterViewOut, class RasterViewIn>
    void assign_blocked(RasterViewOut& out, const RasterViewIn& in, int block_row_size, int block_col_size)
    {
      assert(out.rows() == in.rows() && out.cols() == in.cols());
      const int blocks_per_row = (in.rows() + block_row_size - 1) / block_row_size;
      const int blocks_per_col = (in.cols() + block_row_size - 1) / block_col_size;
      int nblocks = blocks_per_row * blocks_per_col;

      for (int block_row = 0; block_row < blocks_per_row; ++block_row) {
        for (int block_col = 0; block_col < blocks_per_col; ++block_col) {

          //auto do_block = [&, block_row, block_col]()
          //{
          const int first_row = block_row * block_row_size;
          const int first_col = block_col * block_col_size;
          const int rows = std::min<int>(in.rows() - first_row, block_row_size);
          const int cols = std::min<int>(in.cols() - first_col, block_col_size);
          auto sub_out = out.sub_raster(first_row, first_col, rows, cols);
          auto sub_in = out.sub_raster(first_row, first_col, rows, cols);

          assign(sub_out, sub_in);
          // };
          // do_block(); // prepared for parallelization
        }
      }
    }

    // Exploiting that we can get the block size of gdal_raster_views.
    template<class T, class RasterViewIn>
    void assign_blocked(gdal_raster_view<T>& out, const RasterViewIn& in)
    {
      int block_row_size, block_col_size;
      out.get_band()->GetBlockSize(&block_col_size, &block_row_size);
      assign_blocked(out, in, block_row_size, block_col_size);
    }
    
  }
}
