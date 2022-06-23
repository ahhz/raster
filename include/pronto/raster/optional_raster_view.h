//
//=======================================================================
// Copyright 2016-2022
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

    template<class Raster>
    auto optionalize(Raster r)
    {
      return transform([](auto v) { return std::optional(v); }, r);
    }

    template<class R>
    using optional_raster_view = decltype(optionalize(std::declval<R>()));
  }
}

