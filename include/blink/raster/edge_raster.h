//
//=======================================================================
// Copyright 2016-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================

#pragma once

#include <blink/raster/pair_raster_view.h>
#include <blink/raster/traits.h>

namespace blink {
  namespace raster {
    
    template<class R>
    using sub_raster = typename traits<R>::sub_raster;

    template<class R>
    using edge_raster_view = pair_raster_view<sub_raster<R>, sub_raster<R> >;

    template<class R> 
    edge_raster_view<R> h_edge(const R& raster)
    {
      auto r1 = raster.sub_raster(0, 0, raster.rows() - 1, raster.cols());
      auto r2 = raster.sub_raster(1, 0, raster.rows() - 1, raster.cols());
      return raster_pair(r1, r2);
    }

    template<class R>
    edge_raster_view<R> v_edge(const R& raster)
    {
      auto r1 = raster.sub_raster(0, 0, raster.rows(), raster.cols() - 1);
      auto r2 = raster.sub_raster(0, 1, raster.rows(), raster.cols() - 1);
      return raster_pair(r1, r2);
    }
  }
}

