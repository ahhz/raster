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

#include <iterator>
#include <utility>

namespace pronto {
  namespace raster {

    template<class... I> 
    class tuple_raster_iterator
    { 
    public:
      tuple_raster_iterator(const I& ...iters) : m_iters(iters...)
      {
      }

      using reference
        = std::tuple< typename std::iterator_traits<I>::reference...>;

      using value_type 
        = std::tuple< typename std::iterator_traits<I>::value_type...>;

      using pointer = void;
      using difference_type = std::ptrdiff_t;
      using iterator_category = std::input_iterator_tag;

      tuple_raster_iterator& operator++()
      {
         increment(tuple_indices{});
         return *this;
      }

      tuple_raster_iterator& operator+=(const difference_type& n)
      {
        increment_step(n,tuple_indices{});
        return *this;
      }
      
      tuple_raster_iterator operator++(int)
      {
        tuple_raster_iterator temp(*this);
        ++(*this);
        return temp;
      }
     
      tuple_raster_iterator operator+(const difference_type& n) const
      {
        tuple_raster_iterator temp(*this);
        temp += n;
        return temp;
      }

      inline reference operator[](std::ptrdiff_t distance) const
      {
        return *(operator+(distance));
      }

      tuple_raster_iterator& operator--()
      {
        decrement(tuple_indices{});
        return *this;
      }

      tuple_raster_iterator operator--(int)
      {
        tuple_raster_iterator temp(*this);
        --(*this);
        return temp;
      }

      tuple_raster_iterator operator-(const difference_type& n) const
      {
        tuple_raster_iterator temp(*this);
        temp -= n;
        return temp;
      }

      tuple_raster_iterator& operator-=(const difference_type& n)
      {
        decrement_step(n, tuple_indices{});
        return *this;
      }

      reference operator*() const
      {
        return dereference(tuple_indices{});
      }

      bool operator==(const tuple_raster_iterator& b) const
      {
        return N==0 || std::get<0>(m_iters) == std::get<0>(b.m_iters);
      }

      bool operator!=(const tuple_raster_iterator& b) const
      {
        return N != 0 && std::get<0>(m_iters) != std::get<0>(b.m_iters);
      }

      bool operator<(const tuple_raster_iterator& b) const
      {
        return N != 0 && std::get<0>(m_iters) < std::get<0>(b.m_iters);
      }

      bool operator>(const tuple_raster_iterator& b) const
      {
        return N != 0 && std::get<0>(m_iters) > std::get<0>(b.m_iters);
      }

      bool operator<=(const tuple_raster_iterator& b) const
      {
        return N != 0 && std::get<0>(m_iters) <= std::get<0>(b.m_iters);
      }

      bool operator>=(const tuple_raster_iterator& b) const
      {
        return N != 0 && std::get<0>(m_iters) >= std::get<0>(b.m_iters);
      }

    private:
      static const std::size_t N = sizeof...(I);
      using tuple_indices = std::make_index_sequence<N>;

      template<std::size_t ...S>
      void increment(std::index_sequence<S...>)
      {
        auto dummy = { (++std::get<S>(m_iters), 0)... };
      }

      template< std::size_t ...S>
      void decrement(std::index_sequence<S...>)
      {
        auto dummy = { (++std::get<S>(m_iters), 0)... };
      }

      template<std::size_t ...S>
      void increment_step(difference_type step, std::index_sequence<S...>)
      {
        auto dummy = { (std::get<S>(m_iters) += step, 0)... };
      }

      template<std::size_t ...S>
      void decrement_step(difference_type step, std::index_sequence<S...>)
      {
        auto dummy = { (std::get<S>(m_iters) -= step, 0)... };
      }

      template<std::size_t ...S>
      reference dereference(std::index_sequence<S...>) const
      {
        return reference(*std::get<S>(m_iters)...);
      }

      std::tuple<I...> m_iters;
    };

    template<class... R> // requires R1 and R2 are RasterView concepts 
    class tuple_raster_view
    {
    public:
      using sub_raster_type = tuple_raster_view<typename traits<R>::sub_raster...>;
   
      tuple_raster_view() = default;

      tuple_raster_view(const R& ...r) 
        : m_rasters(r...)
      { }

      using iterator = tuple_raster_iterator<
        typename traits<R>::iterator...>;
      
      using const_iterator = tuple_raster_iterator<
        typename traits<R>::const_iterator...>;
      
      iterator begin() 
      {
        return begin(tuple_indices{});
      }

      iterator end() 
      {
        return end(tuple_indices{});
      }

      const_iterator begin() const
      {
        return begin(tuple_indices{});
      }

      const_iterator end() const
      {
        return end(tuple_indices{});
      }

      int rows() const
      {
        if (N == 0) return 0;
        return std::get<0>(m_rasters).rows();
      }

      int cols() const
      {
        if (N == 0) return 0;
        return std::get<0>(m_rasters).cols();
      }

      int size() const
      {
        if (N == 0) return 0;
        return std::get<0>(m_rasters).size();
      }

      sub_raster_type
        sub_raster(int start_row, int start_col, int rows, int cols) const
      {
        return sub_raster(tuple_indices{}, start_row, start_col, rows, cols);
      }

    private:
      static const std::size_t N = sizeof...(R);
      using tuple_indices = std::make_index_sequence<N>;


      template<std::size_t ...S>
      iterator begin(std::index_sequence<S...>)
      {
        return iterator(std::get<S>(m_rasters).begin()...);
      }

      template<std::size_t ...S>
      iterator end(std::index_sequence<S...>)
      {
        return iterator(std::get<S>(m_rasters).end()...);
      }

      template<std::size_t ...S>
      const_iterator begin(std::index_sequence<S...>) const
      {
        return const_iterator(std::get<S>(m_rasters).begin()...);
      }

      template<std::size_t ...S>
      const_iterator end(std::index_sequence<S...>) const
      {
        return const_iterator(std::get<S>(m_rasters).end()...);
      }

      template< std::size_t ...S>
      sub_raster_type sub_raster(std::index_sequence<S...>
        , int start_row, int start_col, int rows, int cols) const
      {
        return tuple_raster_view<typename traits<R>::sub_raster...>
          (std::get<S>(m_rasters).sub_raster
          (start_row, start_col, rows, cols)...);
      }
      std::tuple<R...> m_rasters;
    };

    template<class... R> // requires these to be RasterViews
    tuple_raster_view<R...> raster_tuple(const R&... r)
    {
      return tuple_raster_view<R...>(r...);
    }
  }
}
