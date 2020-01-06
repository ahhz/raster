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

#include <pronto/raster/index_sequence.h>
#include <pronto/raster/reference_proxy.h>

#include <pronto/raster/traits.h>

#include <iterator>
#include <utility>

namespace pronto {
  namespace raster {
    namespace detail {
      void reinterpret_tuple_to_value_help(char* buffer) {}

      template<class Source, class... Rest>
      void reinterpret_tuple_to_value_help(char* buffer, const Source& source,
        const Rest&... rest)
      {
        std::memcpy(buffer, &source, sizeof(Source));
        reinterpret_tuple_to_value_help(buffer + sizeof(Source), rest...);
      }

      template<class Tuple, std::size_t... I>
      void reinterpret_tuple_to_value_help(char* buffer, const Tuple& t,
        std::integer_sequence<std::size_t, I...>)
      {
        reinterpret_tuple_to_value_help(buffer, std::get<I>(t)...);
      }




      void reinterpret_value_to_tuple_help(const char*) {}

      template<class Element, class... Rest>
      void reinterpret_value_to_tuple_help(const char* as_bytes, Element& element,
        Rest&... rest)
      {
        std::memcpy(&element, as_bytes, sizeof(Element));
        reinterpret_value_to_tuple_help(as_bytes + sizeof(Element), rest...);
      }

      template<class Tuple, std::size_t... I>
      void reinterpret_value_to_tuple_help(const char* as_bytes, Tuple& as_tuple,
        std::integer_sequence<std::size_t, I...>)
      {
        reinterpret_value_to_tuple_help(as_bytes, std::get<I>(as_tuple)...);
      }
    }

    template<class Value, class... Element>
    void  reinterpret_value_to_tuple(const Value& value,
      std::tuple<Element...>& tuple)
    {
      const char* bytes = reinterpret_cast<const char*>(&value);
      detail::reinterpret_value_to_tuple_help(bytes, tuple,
        std::make_index_sequence<sizeof...(Element)>{});
    }

    template<class Value, class... Element>
    void reinterpret_tuple_to_value(const std::tuple<Element...>& element,
      Value& value)
    {
      char* bytes = reinterpret_cast<char*>(&value);
      detail::reinterpret_tuple_to_value_help(bytes, element,
        std::make_index_sequence<sizeof...(Element)>{});
    }



    template<class T, class... I> 
    class reinterpret_raster_iterator
    { 
    public:
      reinterpret_raster_iterator(const I& ...iters) : m_iters(iters...)
      {
      }
      
      

      using reference = reference_proxy<reinterpret_raster_iterator>;

      using value_type = T;

      using tuple_reference
        = std::tuple< typename std::iterator_traits<I>::reference...>;

      using tuple_value_type
        = std::tuple< typename std::iterator_traits<I>::value_type...>;


      T get() const
      {
        tuple_value_type tuple = dereference(tuple_indices{});
        T value;
        reinterpret_tuple_to_value(tuple, value);
        return value;
      }

      void put(T value)
      {
        tuple_value_type tuple;
        reinterpret_value_to_tuple(value, tuple);
        dereference(tuple_indices{}) = tuple;
      }

      using pointer = void;
      using difference_type = std::ptrdiff_t;
      using iterator_category = std::input_iterator_tag;

      reinterpret_raster_iterator& operator++()
      {
         increment(tuple_indices{});
         return *this;
      }

      reinterpret_raster_iterator& operator+=(const difference_type& n)
      {
        increment_step(n,tuple_indices{});
        return *this;
      }
      
      reinterpret_raster_iterator operator++(int)
      {
        reinterpret_raster_iterator temp(*this);
        ++(*this);
        return temp;
      }
     
      reinterpret_raster_iterator operator+(const difference_type& n) const
      {
        reinterpret_raster_iterator temp(*this);
        temp += n;
        return temp;
      }

      // Not supported 
      //inline reference operator[](std::ptrdiff_t distance) const
      //{
      //  return *(operator+(distance));
      //}

      reinterpret_raster_iterator& operator--()
      {
        decrement(tuple_indices{});
        return *this;
      }

      reinterpret_raster_iterator operator--(int)
      {
        reinterpret_raster_iterator temp(*this);
        --(*this);
        return temp;
      }

      reinterpret_raster_iterator operator-(const difference_type& n) const
      {
        reinterpret_raster_iterator temp(*this);
        temp -= n;
        return temp;
      }

      reinterpret_raster_iterator& operator-=(const difference_type& n)
      {
        decrement_step(n, tuple_indices{});
        return *this;
      }

      reference operator*() const
      {
        return reference{ *this };
      }

      bool operator==(const reinterpret_raster_iterator& b) const
      {
        return N==0 || std::get<0>(m_iters) == std::get<0>(b.m_iters);
      }

      bool operator!=(const reinterpret_raster_iterator& b) const
      {
        return N != 0 && std::get<0>(m_iters) != std::get<0>(b.m_iters);
      }

      bool operator<(const reinterpret_raster_iterator& b) const
      {
        return N != 0 && std::get<0>(m_iters) < std::get<0>(b.m_iters);
      }

      bool operator>(const reinterpret_raster_iterator& b) const
      {
        return N != 0 && std::get<0>(m_iters) > std::get<0>(b.m_iters);
      }

      bool operator<=(const reinterpret_raster_iterator& b) const
      {
        return N != 0 && std::get<0>(m_iters) <= std::get<0>(b.m_iters);
      }

      bool operator>=(const reinterpret_raster_iterator& b) const
      {
        return N != 0 && std::get<0>(m_iters) >= std::get<0>(b.m_iters);
      }

    private:
      static const std::size_t N = sizeof...(I);
      using tuple_indices = make_index_sequence<N>;

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
      tuple_reference dereference(Pack<S...>) const
      {
        return tuple_reference(*std::get<S>(m_iters)...);
      }

      std::tuple<I...> m_iters;
    };

    template<class T, class... R> // requires R1 and R2 are RasterView concepts 
    class reinterpret_raster_view
    {
    public:
      using sub_raster_type = reinterpret_raster_view<T, typename traits<R>::sub_raster...>;
   
      reinterpret_raster_view() = default;

      reinterpret_raster_view(const R& ...r)
        : m_rasters(r...)
      { }

      using iterator = reinterpret_raster_iterator<T, 
        typename traits<R>::iterator...>;
      
      using const_iterator = reinterpret_raster_iterator<T, 
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

      long long size() const
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
      using tuple_indices = make_index_sequence<N>;


      template<template<std::size_t...> class Pack, std::size_t ...S>
      iterator begin(Pack<S...>)
      {
        return iterator(std::get<S>(m_rasters).begin()...);
      }

      template<template<std::size_t...> class Pack, std::size_t ...S>
      iterator end(Pack<S...>)
      {
        return iterator(std::get<S>(m_rasters).end()...);
      }

      template<template<std::size_t...> class Pack, std::size_t ...S>
      const_iterator begin(Pack<S...>) const
      {
        return const_iterator(std::get<S>(m_rasters).begin()...);
      }

      template<template<std::size_t...> class Pack, std::size_t ...S>
      const_iterator end(Pack<S...>) const
      {
        return const_iterator(std::get<S>(m_rasters).end()...);
      }

      template<template<std::size_t...> class Pack, std::size_t ...S>
      sub_raster_type sub_raster(Pack<S...>
        , int start_row, int start_col, int rows, int cols) const
      {
        return tuple_raster_view<typename traits<R>::sub_raster...>
          (std::get<S>(m_rasters).sub_raster
          (start_row, start_col, rows, cols)...);
      }
      std::tuple<R...> m_rasters;
    };

    template<class T, class... R> // requires these to be RasterViews
    reinterpret_raster_view<T, R...> reinterpret_rasters(const R&... r)
    {
      return reinterpret_raster_view<T, R...>(r...);
    }
  }
}
