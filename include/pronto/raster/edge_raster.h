//
//=======================================================================
// Copyright 2016-2022
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================

#pragma once

#include <pronto/raster/pair_raster_view.h>

namespace pronto {
  namespace raster {
    
    template<class R> 
    auto h_edge(const R& raster)
    {
      auto r1 = raster.sub_raster(0, 0, raster.rows() - 1, raster.cols());
      auto r2 = raster.sub_raster(1, 0, raster.rows() - 1, raster.cols());
      return pair_raster_view<R>(r1, r2);
    }

    template<class R>
    auto v_edge(const R& raster)
    {
      auto r1 = raster.sub_raster(0, 0, raster.rows(), raster.cols() - 1);
      auto r2 = raster.sub_raster(0, 1, raster.rows(), raster.cols() - 1);
      return pair_raster_view<R>(r1, r2);
    }

    template<class R>
    using edge_raster_view = decltype(h_edge(std::declval<R>()));
  }
}

