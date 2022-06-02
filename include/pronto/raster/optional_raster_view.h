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

#include <pronto/raster/transform_raster_view.h>
#include <optional>
namespace pronto {
  namespace raster {

    struct make_optional_value
    {
      template<class T>
      std::optional<T> operator()(const T& v) const  
      {
        return std::optional<T>(v);
      }
    };

   // template <class Raster>
  //  using optional_raster_view = transform_raster_view<
   //   make_optional_value, Raster>;
	
    template<class Raster>
    auto optionalize(Raster r)
    {
      return transform(make_optional_value{}, r);
    }
  }
}

