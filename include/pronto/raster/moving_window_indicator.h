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

#include <pronto/raster/circular_edge_window_view.h>
#include <pronto/raster/circular_window_view.h>
#include <pronto/raster/patch_raster_transform.h>
#include <pronto/raster/rectangle_edge_window_view.h>
#include <pronto/raster/square_window_view.h>

#include <vector>

namespace pronto {
  namespace raster {
    
    struct square
    {
      square() = default;
      square(int radius) : radius(radius) {}
      int radius;
    };

    struct circle
    {
      circle() = default;
      circle(double radius) : radius(radius) {}
      double radius;
    };

    struct edge_square
    {
      edge_square() = default;
      edge_square(int radius) : radius(radius) {}
      int radius;
    };

    struct edge_circle
    {
      edge_circle() = default;
      edge_circle(double radius) : radius(radius) {}
      double radius;
    };
    
    struct patch_square
    {
      patch_square() = default;
      patch_square(int radius) : radius(radius) {}
      int radius;
    };

    struct patch_circle
    {
      patch_circle() = default;
      patch_circle(double radius) : radius(radius) {}
      double radius;
    };

    template<class Raster, class IndicatorGenerator>
    auto moving_window_indicator(const Raster& raster, const square& window
      , const IndicatorGenerator& indicator_generator)->
      decltype(extract(make_square_window_view(raster, window.radius
        , indicator_generator) ) )
    {
      return extract(make_square_window_view(raster, window.radius
        , indicator_generator));
    }

    template<class Raster, class IndicatorGenerator>
    auto moving_window_indicator(const Raster& raster, const circle& window
      , const IndicatorGenerator& indicator_generator)->
      decltype(extract(make_circular_window_view(raster, window.radius
        , indicator_generator) ) )
    {
      return extract(make_circular_window_view(raster, window.radius
        , indicator_generator) );
    }


    template<class Raster, class IndicatorGenerator>
    auto moving_window_indicator(const Raster& raster, const edge_circle& window
      , const IndicatorGenerator& indicator_generator)->
      decltype(extract(make_circular_edge_window_view(raster, window.radius
        , indicator_generator)))
    {
      return extract(make_circular_edge_window_view(raster, window.radius
        , indicator_generator));
    }

    template<class Raster, class IndicatorGenerator>
    auto moving_window_indicator(const Raster& raster, const edge_square& window
      , const IndicatorGenerator& indicator_generator)->
      decltype(extract(make_square_edge_window_view(raster, window.radius
        , indicator_generator)))
    {
      return extract(make_square_edge_window_view(raster, window.radius
        , indicator_generator));
    }

    template<class Raster, class IndicatorGenerator, class Contiguity>
    auto moving_window_indicator(const Raster& raster, const patch_square& window
      , const IndicatorGenerator& indicator_generator, Contiguity contiguity)->
      decltype(extract(make_square_window_view(patch_raster(raster, contiguity)
        , window.radius, indicator_generator)))
    {
      return extract(make_square_window_view(patch_raster(raster, contiguity)
        , window.radius, indicator_generator));
    }

    template<class Raster, class IndicatorGenerator, class Contiguity>
    auto moving_window_indicator(const Raster& raster, const patch_circle& window
      , const IndicatorGenerator& indicator_generator, Contiguity contiguity)->
      decltype(extract(make_circular_window_view(patch_raster(raster, contiguity)
        , window.radius, indicator_generator)))
    {
      return extract(make_circular_window_view(patch_raster(raster, contiguity)
        , window.radius, indicator_generator));
    }

  }
}
