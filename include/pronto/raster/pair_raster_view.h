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

#include <pronto/raster/traits.h>
#include <pronto/raster/iterator_facade.h>

#include <ranges>
#include <utility>


namespace pronto {
  namespace raster {
   
    template<class I1, class I2> 
    class pair_raster_iterator : public iterator_facade< pair_raster_iterator<I1,I2>>
    {
    public:
      static const bool is_mutable = false;
      static const bool is_multipass = true;

     pair_raster_iterator() = default;

      pair_raster_iterator(const I1& a, const I2& b) : m_iters(a, b)
      {
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
      
      auto dereference() const
      {
        return std::pair(*m_iters.first, *m_iters.second);
      }

      bool equal_to(const pair_raster_iterator& b) const
      {
        return m_iters.first == b.m_iters.first;
      }

      bool distance_to(const pair_raster_iterator& b) const
      {
        return  b.m_iters.first - m_iters.first;
      }
      
    private:
      std::pair<I1, I2> m_iters;
    };

    template<class R1, class R2> // requires R1 and R2 are RasterView concepts 
    class pair_raster_view : public std::ranges::view_interface< pair_raster_view<R1, R2> >
    {
    public:
      pair_raster_view() = default;

      pair_raster_view(const R1& r1, const R2& r2) 
        : m_rasters(r1, r2)
      { }
     
      auto begin() 
      {
        return pair_raster_iterator(m_rasters.first.begin(), m_rasters.second.begin() );
      }

      auto end()
      {
        return pair_raster_iterator(m_rasters.first.end(), m_rasters.second.end());
      }

      auto begin() const
      {
        return pair_raster_iterator( m_rasters.first.begin(), m_rasters.second.begin());
      }

      auto end() const
      {
        return pair_raster_iterator( m_rasters.first.end(), m_rasters.second.end());
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

      auto
        sub_raster(int start_row, int start_col, int rows, int cols) const
      {
        return pair_raster_view
          ( m_rasters.first.sub_raster(start_row, start_col, rows, cols)
          , m_rasters.second.sub_raster(start_row, start_col, rows, cols));
      }

    private:
      std::pair<R1, R2> m_rasters;
    };

    template<class R1, class R2> // requires these to be RasterViews
    auto raster_pair(const R1& r1, const R2& r2)
    {
      return pair_raster_view(r1, r2);
    }
  }
}

