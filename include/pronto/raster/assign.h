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


#include <pronto/raster/gdal_raster_view.h>
#include <pronto/raster/raster.h>

#include <ranges>
#include <variant>


namespace pronto {
  namespace raster {
   
    template<class RasterTo, class RasterFrom> // for non-conform raster : temp solution
    void assign(RasterTo& to, const RasterFrom& from)
    {
      using in_value_type = std::ranges::range_value_t<RasterFrom>;
      using out_value_type = std::ranges::range_value_t<RasterTo>;
      auto i = from.begin();
      auto i_end = from.end();
      auto j = to.begin();
      for (; i != i_end; ++i, ++j) 
      {
        *j = recursive_get_value(static_cast<in_value_type>(*i));
      }
    }

    template<RasterConcept RasterTo, RasterConcept RasterFrom> // only really needs to be a range
    void assign(RasterTo& to, const RasterFrom& from)
    {
      using in_value_type = std::ranges::range_value_t<RasterFrom>;
      using out_value_type = std::ranges::range_value_t<RasterTo>;
      using inner_out_value_type = recursive_optional_value_type<out_value_type>;
      
      auto i = from.begin();
      auto i_end = from.end();
      auto j = to.begin();

      // Lots of static-casting to avoid warnings when unequal rasters are assigned.
      for (; i != i_end; ++i, ++j)
      {
        if constexpr (is_optional_v<in_value_type> && is_optional_v<out_value_type>)
        {
          if (recursive_is_initialized(static_cast<in_value_type>(*i))) {
            *j = static_cast<inner_out_value_type>(recursive_get_value(static_cast<in_value_type>(*i)));
          }
          else {
            *j = out_value_type{};
          }
        }
        else if constexpr (is_optional_v<in_value_type> && !is_optional_v<out_value_type>)
        {
          if (recursive_is_initialized(static_cast<in_value_type>(*i))) {
            *j = static_cast<out_value_type>(recursive_get_value(static_cast<in_value_type>(*i)));
          }
          else {
            assert(false); // assigning an optional to a non-optional
            *j = out_value_type{};
          }
        }
        else if constexpr (!is_optional_v<in_value_type> && is_optional_v<out_value_type>)
        {
          *j = static_cast<inner_out_value_type>(static_cast<in_value_type>(*i));
        }
        else//constexpr (!is_optional_v<in_value_type> && !is_optional_v<out_value_type>)
        {
          *j = static_cast<out_value_type>(static_cast<in_value_type>(*i));
        }


      }
       //std::copy(from.begin(), from.end(), to.begin());
    }

    template<RasterConcept RasterTo, RasterVariantConcept RasterFrom> // only needs to be a range
    void assign(RasterTo& to, const RasterFrom& from)
    {
      std::visit([](auto raster) {assign(to, raster); }, from);
    }

    template<RasterVariantConcept RasterTo, RasterConcept RasterFrom> // only needs to be a range
    void assign(RasterTo& to, const RasterFrom& from)
    {
      std::visit([](auto raster) { assign(raster, from); }, to);
    }

    template<RasterVariantConcept RasterTo, RasterVariantConcept RasterFrom> // only needs to be a range
    void assign(RasterTo& to, const RasterFrom& from)
    {
      std::visit([](auto a, auto b) { assign(a, b); }, to, from);
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
          auto sub_in = in.sub_raster(first_row, first_col, rows, cols);

          assign(sub_out, sub_in);
          // };
          // do_block(); // prepared for parallelization
        }
      }
    }

    // Exploiting that we can get the block size of gdal_raster_views.
    template<class T, iteration_type IType, access AType, class RasterViewIn>
    void assign_blocked(gdal_raster_view<T, IType, AType>& out, const RasterViewIn& in)
    {
      int block_row_size, block_col_size;
      out.get_band()->GetBlockSize(&block_col_size, &block_row_size);
      assign_blocked(out, in, block_row_size, block_col_size);
    }
    
  }
}
