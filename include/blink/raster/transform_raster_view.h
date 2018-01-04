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

#include <blink/raster/index_sequence.h> 
#include <blink/raster/optional.h>
#include <blink/raster/traits.h>

#include <functional>
#include <iterator>
#include <utility>

namespace blink {
  namespace raster {
    
    // Now casting all inputs to the function to their value_type
    // this means that the proxy references will be cast, and therefore
    // all iterators are non-mutable.
   
    template<class View, class... I> 
    class transform_raster_iterator
    {
    private: 
      static const std::size_t N = sizeof...(I);
      using tuple_indices = make_index_sequence<N>;

    public:
      transform_raster_iterator(const View& view, const I& ...iters)
        : m_view(&view), m_iters(iters...)
      {
      }

      transform_raster_iterator() = default;
      //~transform_raster_iterator() = default;
      transform_raster_iterator(const transform_raster_iterator&) = default;
      transform_raster_iterator(transform_raster_iterator&&) = default;
      transform_raster_iterator& operator=(const transform_raster_iterator&) = default; //delete because lambdas cannot be assigned
      transform_raster_iterator& operator=(transform_raster_iterator&&) = default;
      
      using value_type = typename View::value_type;
      using reference = value_type;
      using pointer = void;
      using difference_type = std::ptrdiff_t;
      using iterator_category = std::output_iterator_tag;
     
      transform_raster_iterator& operator++()
      {
         increment(tuple_indices{});
         return *this;
      }
      
      transform_raster_iterator operator++(int)
      {
        transform_raster_iterator temp(*this);
        ++(*this);
        return temp;
      }

      transform_raster_iterator& operator+=(const difference_type& n)
      {
        increment_step(n, tuple_indices{});
        return *this;
      }

      transform_raster_iterator operator+(const difference_type& n) const
      {
        tuple_raster_iterator temp(*this);
        temp += n;
        return temp;
      }

      transform_raster_iterator& operator--()
      {
        decrement(tuple_indices{});
        return *this;
      }

      transform_raster_iterator operator--(int)
      {
        transform_raster_iterator temp(*this);
        --(*this);
        return temp;
      }

      transform_raster_iterator& operator-=(const difference_type& n)
      {
        decrement_step(n, tuple_indices{});
        return *this;
      }

      transform_raster_iterator operator-(const difference_type& n) const
      {
        tuple_raster_iterator temp(*this);
        temp -= n;
        return temp;
      }

      reference operator*() const
      {
        return dereference(tuple_indices{});
      }

      reference operator[](std::ptrdiff_t distance) const
      {
        return *(operator+(distance));
      }

      bool operator==(const transform_raster_iterator& b) const
      {
        return N==0 || std::get<0>(m_iters) == std::get<0>(b.m_iters);
      }

      bool operator!=(const transform_raster_iterator& b) const
      {
        return N != 0 && std::get<0>(m_iters) != std::get<0>(b.m_iters);
      }

      bool operator<(const transform_raster_iterator& b) const
      {
        return N != 0 && std::get<0>(m_iters) < std::get<0>(b.m_iters);
      }

      bool operator>(const transform_raster_iterator& b) const
      {
        return N != 0 && std::get<0>(m_iters) > std::get<0>(b.m_iters);
      }

      bool operator<=(const transform_raster_iterator& b) const
      {
        return N != 0 && std::get<0>(m_iters) <= std::get<0>(b.m_iters);
      }

      bool operator>=(const transform_raster_iterator& b) const
      {
        return N != 0 && std::get<0>(m_iters) >= std::get<0>(b.m_iters);
      }


    private:
      //
      //https://stackoverflow.com/questions/16387354/template-tuple-calling-a-function-on-each-element
      //
      // actually we know that Pack is index_sequence, but this is more future proof 

      template<template<std::size_t...> class Pack, std::size_t ...S>
      void increment(Pack<S...>)
      {
        auto dummy = { (++std::get<S>(m_iters), 0)... };
      }

      template<template<std::size_t...> class Pack, std::size_t ...S>
      void decrement(Pack<S...>)
      {
        auto dummy = { (++std::get<S>(m_iters), 0)... };
      }

      template<template<std::size_t...> class Pack, std::size_t ...S>
      void increment_step(difference_type step, Pack<S...>)
      {
        auto dummy = { (std::get<S>(m_iters) += step, 0)... };
      }

      template<template<std::size_t...> class Pack, std::size_t ...S>
      void decrement_step(difference_type step, Pack<S...>)
      {
        auto dummy = { (std::get<S>(m_iters) -= step, 0)... };
      }

      template<template<std::size_t...> class Pack, std::size_t ...S>
      reference dereference(Pack<S...>) const
      {
        // #pragma warning( push )
        // #pragma warning( disable : 4244 ) // suppressing warnings due to casts
        // #pragma warning( disable : 4800 ) // suppressing warnings due to casts
        auto& f = *(m_view->m_function);
        return f(
          static_cast<typename
          std::iterator_traits<typename std::tuple_element<S, std::tuple<I...>>::type>::value_type>
          (*std::get<S>(m_iters))...);
        // #pragma warning( pop )
      }


      const View* m_view;
      std::tuple<I...> m_iters;
    };

    template<class F, class... R> 
    class transform_raster_view
    {
    private:
      using value_type = decltype(std::declval<F>()(std::declval<typename traits<R>::value_type>()...));
      
      using function_type = F;
    
      static const std::size_t N = sizeof...(R);
      using tuple_indices = make_index_sequence<N>;

    public:
      transform_raster_view() = default;
      transform_raster_view(const transform_raster_view&) = default;
      transform_raster_view(transform_raster_view&&) = default;
      transform_raster_view& operator=(const transform_raster_view&) = default; // cannot assign lambdas
      transform_raster_view& operator=(transform_raster_view&&) = default;

      transform_raster_view(F f, R ...r) : m_function(f), m_rasters(r...)
      { }

      using const_iterator = transform_raster_iterator<transform_raster_view,
        typename traits<R>::const_iterator...>;
      
      using sub_raster_type = transform_raster_view<function_type, typename traits<R>::sub_raster...>;


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
      
      // Not allowing the function to change values in the rasters
      template<template<std::size_t...> class Pack, std::size_t ...S>
      const_iterator begin(Pack<S...>) const
      {
        return const_iterator(*this, std::get<S>(m_rasters).begin()...);
      }

      template<template<std::size_t...> class Pack, std::size_t ...S>
      const_iterator end(Pack<S...>) const
      {
        return const_iterator(*this, std::get<S>(m_rasters).end()...);
      }

      template<template<std::size_t...> class Pack, std::size_t ...S>
      sub_raster_type sub_raster(Pack<S...>
        , int start_row, int start_col, int rows, int cols) const
      {
        return sub_raster_type
        (*m_function, std::get<S>(m_rasters).sub_raster
        (start_row, start_col, rows, cols)...);
      }

      std::tuple<R...> m_rasters;
    private:
      friend class const_iterator;
      //friend class iterator;

      mutable optional<function_type> m_function;
    };

    template<class F, class... R> // requires these to be RasterViews
    transform_raster_view<F, R...>
      transform(F f, R... r)
    {
      return transform_raster_view<typename std::decay<F>::type, R...>
        (std::forward<F>(f), r...);
    }
  }
}
