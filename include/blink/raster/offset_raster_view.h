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

#include <blink/raster/padded_raster_view.h>
#include <blink/raster/traits.h>

#include <cmath>

namespace blink {
  namespace raster {
	  
    template<typename Raster> 
    using offset_raster_view = padded_raster_view<typename traits<Raster>::sub_raster>;

    template<typename Raster> 
    offset_raster_view<Raster>
      offset(Raster raster, int row_offset, int col_offset, 
        const typename traits<Raster>::value_type& pad_value)
    {
      auto sub = raster.sub_raster
        ( std::max(0, row_offset)
        , std::max(0, col_offset)
        , raster.rows() - std::abs(row_offset)
        , raster.cols() - std::abs(col_offset));

      return pad(sub
        , std::max(0, -row_offset)
        , std::max(0, row_offset)
        , std::max(0, -col_offset)
        , std::max(0, col_offset)
        , pad_value);
    }
  }
}


