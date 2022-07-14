//
//=======================================================================
// Copyright 2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once

#include <pronto/raster/pair_raster_view.h>
#include <pronto/raster/traits.h>
#include <pronto/raster/iterator_facade.h>
#include <pronto/raster/reference_proxy.h>
#include <utility>

namespace pronto {
  namespace raster {
   
    template<class Value, class Weight>
    struct weighted_value 
    {
      weighted_value() : m_value(), m_weight()
      {}

      weighted_value(const Value& v, const Weight& w) : m_value(v), m_weight(w)
      {}

      template<class V, class W>
      weighted_value(const weighted_value<V, W>& wv) : m_value(wv.m_value), m_weight(wv.m_weight)
      {}

      Value m_value;
      Weight m_weight;
    };

    template<class Value, class Weight>
    weighted_value<Value, Weight> make_weighted_value(const Value& v, const Weight& w)
    {
      return weighted_value<Value, Weight>(v, w);
    }

    template<class I1, class I2> 
    class weighted_input_iterator : public iterator_facade<weighted_input_iterator<I1, I2> > {
    public:

      using value_type = weighted_value<std::iter_value_t<I1>, std::iter_value_t<I2> >;

      weighted_input_iterator(const I1& a, const I1& b) : m_iters(a, b)
      {
      }
      static const bool is_single_pass = true;
      static const bool is_mutable = false;

      auto get() const
      {
        return value_type(*m_iters.first, *m_iters.second);

      }
      void put(const value_type& v)
      {
        *m_iters.first = v.m_value;
        *m_iters.second = v.m_weight;
      }
      
      void increment()
      {
        ++m_iters.first;
        ++m_iters.second;
      }

      void decrement()
      {
        --m_iters.first;
        --m_iters.second;
      }
      void advance(std::ptrdiff_t n)
      {
        m_iters.first += n;
        m_iters.second += n;
      }

      auto dereference() const {
        if constexpr (is_mutable)
        {
          if constexpr (is_single_pass)
          {
            return put_get_proxy_reference<const weighted_input_iterator&>(*this);
          }
          else {
            return put_get_proxy_reference<weighted_input_iterator>(*this);
          }
        }
        else {
          return get();
        }
      }
      
      bool equal_to(const weighted_input_iterator& b) const
      {
        return m_iters.first == b.m_iters.first;
      }

      bool distance_to(const weighted_input_iterator& b) const
      {
        return  b.m_iters.first - m_iters.first;
      }

    private:
      std::pair<I1, I2> m_iters;
    };

    template<class R1, class R2> // requires R1 and R2 are RasterView concepts 
    class weighted_input_view : public std::ranges::view_interface< weighted_input_view<R1, R2> >
    {
    public:
      weighted_input_view(const R1& r1, const R2& r2)
        : m_rasters(r1, r2)
      { }
     
      auto begin() const
      {
        return weighted_input_iterator(m_rasters.first.begin(), m_rasters.second.begin() );
      }

      auto end() const
      {
        return weighted_input_iterator(m_rasters.first.end(), m_rasters.second.end());
      }
      
      int rows() const
      {
        return m_rasters.first.rows();
      }

      int cols() const
      {
        return m_rasters.first.cols();
      }

      int size() const
      {
        return m_rasters.first.size();
      }

      auto sub_raster(int start_row, int start_col, int rows, int cols) const
      {
        return weighted_input_view
          < typename traits<R1>::sub_raster
          , typename traits<R2>::sub_raster >
          ( m_rasters.first.sub_raster(start_row, start_col, rows, cols)
          , m_rasters.second.sub_raster(start_row, start_col, rows, cols));
      }

    private:
      std::pair<R1, R2> m_rasters;
    };

    template<class R1, class R2> // requires these to be RasterViews
    auto weighted_raster(const R1& r1, const R2& r2)
    {
      return weighted_input_view(r1, r2);
    }
  }
}
