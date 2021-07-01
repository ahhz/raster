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

#include <iterator>
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
    class weighted_input_iterator
      : public std::iterator
      < std::input_iterator_tag
      , weighted_value< typename std::iterator_traits<I1>::value_type
        , typename std::iterator_traits<I2>::value_type>
      , void // no distance type
      , void // no pointer type
      , weighted_value< typename std::iterator_traits<I1>::reference
        , typename std::iterator_traits<I1>::reference> >
   {
    public:
      weighted_input_iterator(const I1& a, const I1& b) : m_iters(a, b)
      {
      }

      using reference = weighted_value
        < typename std::iterator_traits<I1>::reference
        , typename std::iterator_traits<I2>::reference>;
      
      using value_type = weighted_value
        < typename std::iterator_traits<I1>::value_type
        , typename std::iterator_traits<I2>::value_type>;

      weighted_input_iterator& operator++()
      {
        ++m_iters.first;
        ++m_iters.second;
         return *this;
      }
      
      weighted_input_iterator operator++(int)
      {
        pair_raster_iterator temp(*this);
        ++(*this);
        return temp;
      }

      reference operator*() const
      {
        return reference(*m_iters.first, *m_iters.second);
      }

      bool operator==(const weighted_input_iterator& b) const
      {
        return m_iters.first == b.m_iters.first;
      }

      bool operator!=(const weighted_input_iterator& b) const
      {
        return m_iters.first != b.m_iters.first;
      }

    private:
      std::pair<I1, I2> m_iters;
    };

    template<class R1, class R2> // requires R1 and R2 are RasterView concepts 
    class weighted_input_view
    {
    public:
      weighted_input_view(const R1& r1, const R2& r2)
        : m_rasters(r1, r2)
      { }
      using iterator = weighted_input_iterator<
        typename traits<R1>::iterator, 
        typename traits<R2>::iterator>;
      
      using const_iterator = weighted_input_iterator<
        typename traits<R1>::const_iterator,
        typename traits<R2>::const_iterator>;

      iterator begin() 
      {
        return iterator(m_rasters.first.begin(), m_rasters.second.begin() );
      }

      iterator end() 
      {
        return iterator(m_rasters.first.end(), m_rasters.second.end());
      }

      const_iterator begin() const
      {
        return const_iterator
          ( m_rasters.first.begin()
          , m_rasters.second.begin());
      }

      const_iterator end() const
      {
        return const_iterator
        ( m_rasters.first.end(), 
          m_rasters.second.end());
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

      weighted_input_view<
        typename traits<R1>::sub_raster, typename traits<R2>::sub_raster >
        sub_raster(int start_row, int start_col, int rows, int cols) const
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
    weighted_input_view<R1, R2> weighted_raster(const R1& r1, const R2& r2)
    {
      return weighted_input_view<R1, R2>(r1, r2);
    }
  }
}
