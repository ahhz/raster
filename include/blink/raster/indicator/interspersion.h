//
//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// TODO: can we adjust this measure to allow weighted samples?

#pragma once

#include <blink/raster/optional.h>

#include <cmath>
#include <set>
#include <map>
#include <utility>

namespace blink {
  namespace raster {

    template<typename T> // value type of the edge sides 
    struct interspersion
    {
    private:
      using edge_type = std::pair<T, T>;
   
    public: 

      interspersion() : m_total(0)
      {}
      
      void add_sample(const edge_type& edge)
      {
        if (edge.first != edge.second) {
          ++m_edges[make_sorted_edge(edge)];
          ++m_total;
        }
      }

      void subtract_sample(const edge_type& edge)
      {
        if (edge.first != edge.second) {
          
          auto i = m_edges.find(make_sorted_edge(edge));

          // can only remove edges that are there
          assert(i != m_edges.end()); 

          --(i->second);
          --m_total;
          if (i->second == 0)  m_edges.erase(i);
        }
      }

      void add_subtotal(const interspersion& subtotal)
      {
        for (auto&& v : subtotal.m_edges) {
          m_edges[v.first] += v.second;
          m_total += v.second;
        }
      }

      void subtract_subtotal(const interspersion& subtotal)
      {
        for (auto&& v : subtotal.m_edges) {
          auto i = m_edges.find(v.first);

          // can only remove edges that are there
          assert(i != m_edges.end() && i->second >= v.second);
     
          i->second -= v.second;
          if (i->second == 0) m_edges.erase(i);
          m_total -= v.second;
        }
      }

      optional<double> extract() const
      {
        double numerator = 0;
        std::set<T> categories;
        int total = 0;
        for (auto&& v : m_edges) {
          categories.insert(v.first.first);
          categories.insert(v.first.second);
          total += v.second;
          const double edge_fraction = static_cast<double>(v.second) / m_total;
          numerator += edge_fraction * std::log(edge_fraction);
        }
        assert(total == m_total);
        const std::size_t m = categories.size();

        // Need at least three categories to calculate interspersion
        if (m < 3) return none;

        const double nominator = -std::log(0.5 * m * (m - 1));
        return numerator / nominator;
      }

    private: 
      
      edge_type make_sorted_edge(const edge_type& edge) const
      {
        edge_type out = edge;
        if (out.first < out.second) std::swap(out.first, out.second);
        return out;
      }      
      
      std::map<edge_type, int> m_edges;
      int m_total;
    };

    template<class T>
    class interspersion_generator
    {
    public:
      using indicator = interspersion<T>;
      indicator operator()() const
      {
        return indicator{};
      }
    };
  }
} 
