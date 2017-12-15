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

#include <blink/raster/optional.h>
#include <blink/raster/transform_raster_view.h>

namespace blink {
  namespace raster {

    struct make_optional_value
    {
      template<class T>
      optional<T> operator()(const T& v) const  
      {
        return optional<T>(v);
      }
    };

    template <class Raster>
    using optional_raster_view = transform_raster_view<
      make_optional_value, Raster>;
	
    template<class Raster>
    optional_raster_view<Raster> optionalize(Raster r)
    {
      return optional_raster_view<Raster>(make_optional_value{}, r);
    }
  }
}

