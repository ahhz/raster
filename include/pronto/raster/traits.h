//
//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once

#include <iterator>

namespace pronto {
  namespace raster {
    
    template<class Raster>
    struct traits
    {
      using iterator = decltype(std::declval<Raster>().begin());
      using const_iterator = decltype(std::declval<const Raster>().begin());
      using value_type = typename std::iterator_traits<iterator>::value_type;
      using sub_raster = decltype(std::declval<Raster>().sub_raster(int{}
        , int{}, int{}, int{}));
    };
  }
}
