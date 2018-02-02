
//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================

#pragma once

#include <pronto/raster/optional.h>
#include <pronto/raster/traits.h>


namespace pronto {
  namespace raster {

    // Edge is the value_type of the edge 
    template<typename Edge> 
    struct edge_density
    {
      using edge_type = Edge;

      void add_sample(const edge_type& edge)
      {
        count_all++;
        if (edge.first != edge.second) {
          count_edge++;
        }
      }

      void subtract_sample(const edge_type& edge)
      {
        count_all--;
        if (edge.first != edge.second) {
          count_edge--;
        }
      }

      void add_subtotal(const edge_density& subtotal)
      {
        count_all += subtotal.count_all;
        count_edge += subtotal.count_edge;
      }

      void subtract_subtotal(const edge_density& subtotal)
      {
        count_all -= subtotal.count_all;
        count_edge -= subtotal.count_edge;
      }

      optional<double> extract() const
      {
        if (count_all > 0) return count_edge / count_all;
        else return optional<double>{};
      }

      double count_all;
      double count_edge;
    };

    // T is the value type fo the raster, not the edge_type 
    template<class T> 
    class edge_density_generator
    {
      using edge_type = std::pair<T, T>;
    public:
      using indicator = edge_density<edge_type>;
      indicator operator()() const
      {
        return indicator{};
      }
    };

  } //moving_window_v2
} // pronto 