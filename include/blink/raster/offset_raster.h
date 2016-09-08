//
//=======================================================================
// Copyright 2016
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#ifndef BLINK_RASTER_OFFSET_RASTER_H_AHZ
#define BLINK_RASTER_OFFSET_RASTER_H_AHZ

#include <blink/raster/padded_raster_view.h>
//#include <blink/raster/gdal_input_iterator.h>
#include <functional>
#include <cmath>

namespace blink {
  namespace raster {
   
    template<typename SubbableRaster>
    auto offset_raster(SubbableRaster&& raster, int row_offset, int col_offset)
      ->decltype(pad_raster(sub_raster(std::forward<SubbableRaster>(raster),
        0, 0, 0, 0), 0, 0, 0, 0))
    {
      auto sub = sub_raster(std::forward<SubbableRaster>(raster)
        , std::max(0, row_offset)
        , std::max(0, col_offset)
        , raster.rows() - std::abs(row_offset)
        , raster.cols() - std::abs(col_offset));

      return pad_raster(std::move(sub)
        , std::max(0, -row_offset)
        , std::max(0, row_offset)
        , std::max(0, -col_offset)
        , std::max(0, col_offset));
    }
  }
}

#endif


