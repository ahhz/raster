//
//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================       
//
// The square_indow_iterator provides a generalized interface for patch
// or cell-based indicators
//
#pragma once

#include <blink/raster/rectangle_window_view.h>

namespace blink {
  namespace raster {
  
    template<class Raster, class IndicatorGenerator>
    rectangle_window_view<Raster, IndicatorGenerator>
      make_square_window_view(Raster raster, int radius
        , IndicatorGenerator indicator_generator)
    {
      using return_type = 
        rectangle_window_view<Raster, IndicatorGenerator >;
      return return_type(raster, radius, radius, radius, radius
        , indicator_generator);
    }
  }
}


