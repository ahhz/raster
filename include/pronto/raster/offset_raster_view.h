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

#include <pronto/raster/padded_raster_view.h>
#include <pronto/raster/traits.h>

#include <cmath>

namespace pronto {
  namespace raster {
	  
    template<typename Raster> 
    auto offset(Raster raster, int row_offset, int col_offset, 
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

    template<typename Raster>
    using offset_raster_view = decltype(offset(std::declval<Raster>(), int{}, int{}));
  }
}


