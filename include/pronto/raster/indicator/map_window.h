
//=======================================================================
// Copyright 2015
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
// Primarily intended for debugging purposes. This gives the full list of 
// values in the window
#pragma once

#include <map>
#include <numeric>
#include <utility>

namespace pronto {
  namespace raster {

    template<typename T> // value type 
    struct map_window
    {
      void add_sample(const T& v)
      {
        m_window[v]++;
      }

      void subtract_sample(const T& v)
      {
        auto iter = m_window.find(v);
        assert(iter != m_window.end());// only remove what has been added
        iter->second--;
        if (iter->second == 0) m_window.erase(iter);
      }

      void add_subtotal(const map_window& subtotal)
      {
        for (auto&& kv : subtotal.m_window) {
          m_window[kv.first] += kv.second;
        }
      }

      void subtract_subtotal(const map_window& subtotal)
      {
        for (auto&& kv : subtotal.m_window)
        {
          auto iter = m_window.find(kv.first);
          assert(iter != m_window.end());// only remove what has been added
          iter->second -= kv.second;
          assert(iter->second >= 0);
          if (iter->second == 0) m_window.erase(iter);
        }
      }

      int extract() const
      {
        int sum = 0;
        for (auto&& kv : m_window) {
          sum += kv.second;
        }
        return sum;
      }

      std::map<T, int> m_window;
     
    };

    template<class T>
    class map_window_generator
    {
     public:
      using indicator = map_window<T>;
      indicator operator()() const
      {
        return indicator{};
      }
    };

    template<class T>
    class map_edge_window_generator
    {
      using edge_type = std::pair<T, T>;
    public:
      using indicator = map_window<edge_type>;
      indicator operator()() const
      {
        return indicator{};
      }
    };

  } //moving_window_v2
} // pronto 