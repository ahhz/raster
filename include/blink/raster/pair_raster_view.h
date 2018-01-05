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

#include <blink/raster/traits.h>

#include <iterator>
#include <utility>


namespace blink {
  namespace raster {
   
    template<class I1, class I2> 
    class pair_raster_iterator
    {
    public:

      using reference = std::pair
        < typename std::iterator_traits<I1>::reference
        , typename std::iterator_traits<I2>::reference>;

      using value_type = std::pair
        < typename std::iterator_traits<I1>::value_type
        , typename std::iterator_traits<I2>::value_type>;

      using pointer = void;

      using difference_type = std::ptrdiff_t;
      
      using iterator_category = std::input_iterator_tag; 

      pair_raster_iterator() = default;

      pair_raster_iterator(const I1& a, const I2& b) : m_iters(a, b)
      {
      }

      pair_raster_iterator& operator++()
      {
        ++m_iters.first;
        ++m_iters.second;
         return *this;
      }
      
      pair_raster_iterator operator++(int)
      {
        pair_raster_iterator temp(*this);
        ++(*this);
        return temp;
      }

      pair_raster_iterator operator+(const difference_type& n) const
      {
		pair_raster_iterator temp(*this);
        temp.m_iters.first += n;
        temp.m_iters.second += n;
        return temp;
      }
      
      pair_raster_iterator& operator+=(const difference_type& n)
      {
        m_iters.first += n;
        m_iters.second += n;
        return *this;
      }


      pair_raster_iterator& operator--()
      {
        --m_iters.first;
        --m_iters.second;
        return *this;
      }

      pair_raster_iterator operator--(int)
      {
		pair_raster_iterator temp(*this);
        --(*this);
        return temp;
      }

      pair_raster_iterator operator-(const difference_type& n) const
      {
		pair_raster_iterator temp(*this);
        temp.m_iters.first -= n;
        temp.m_iters.second -= n;
        return temp;
      }

      pair_raster_iterator& operator-=(const difference_type& n)
      {
        m_iters.first -= n;
        m_iters.second -= n;
        return *this;
      }
     
      reference operator*() const
      {
        return reference(*m_iters.first, *m_iters.second);
      }

      reference operator[](std::ptrdiff_t distance) const
      {
        return *(operator+(distance));
      }

      bool operator==(const pair_raster_iterator& b) const
      {
        return m_iters.first == b.m_iters.first;
      }

      bool operator!=(const pair_raster_iterator& b) const
      {
        return m_iters.first != b.m_iters.first;
      }

      bool operator<(const pair_raster_iterator& b) const
      {
        return m_iters.first < b.m_iters.first;
      }

      bool operator>(const pair_raster_iterator& b) const
      {
        return m_iters.first > b.m_iters.first;
      }

      bool operator<=(const pair_raster_iterator& b) const
      {
        return m_iters.first <= b.m_iters.first;
      }

      bool operator>=(const pair_raster_iterator& b) const
      {
        return m_iters.first >= b.m_iters.first;
      }

    private:
      std::pair<I1, I2> m_iters;
    };

    template<class R1, class R2> // requires R1 and R2 are RasterView concepts 
    class pair_raster_view
    {
    public:
      pair_raster_view() = default;

      pair_raster_view(const R1& r1, const R2& r2) 
        : m_rasters(r1, r2)
      { }
      using iterator = pair_raster_iterator<
        typename traits<R1>::iterator, 
        typename traits<R2>::iterator>;
      
      using const_iterator = pair_raster_iterator<
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

      pair_raster_view<
        typename traits<R1>::sub_raster, typename traits<R2>::sub_raster >
        sub_raster(int start_row, int start_col, int rows, int cols) const
      {
        return pair_raster_view
          < typename traits<R1>::sub_raster
          , typename traits<R2>::sub_raster >
          ( m_rasters.first.sub_raster(start_row, start_col, rows, cols)
          , m_rasters.second.sub_raster(start_row, start_col, rows, cols));
      }

    private:
      std::pair<R1, R2> m_rasters;
    };

    template<class R1, class R2> // requires these to be RasterViews
    pair_raster_view<R1, R2> raster_pair(const R1& r1, const R2& r2)
    {
      return pair_raster_view<R1, R2>(r1, r2);
    }
  }
}

