//
//=======================================================================
// Copyright 2017-2022
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

// This is not being used / tested

#pragma once


#include <pronto/raster/iterator_facade.h>
#include <pronto/raster/traits.h>

#include <tuple>

namespace pronto {
  namespace raster {

    template<class... I> 
    class tuple_raster_iterator : public iterator_facade< tuple_raster_iterator<I...>>
    {
    public:
      static const std::size_t N = sizeof...(I);

      static const bool is_single_pass = false;
      static const bool is_mutable = true;

      using value_type = std::tuple<std::iter_value_t<I>...>;

      tuple_raster_iterator(const I& ...iters) : m_iters(iters...)
      {
      }

      auto dereference() const {
        return std::apply([](auto&&... iters) {return std::make_tuple(*iters...); }, m_iters);
      }

      void increment() {
        std::apply([](auto&&... iters) {(++iters, ...); }, m_iters);
      }
      void decrement() {
        std::apply([](auto&&... iters) {(--iters, ...); }, m_iters);
      }

      void advance(std::ptrdiff_t offset) {
        std::apply([&](auto&&... iters) {((iters+=offset), ...); }, m_iters);
      }

      bool equal_to(const tuple_raster_iterator& other) const {
        return N > 0 && std::get<0>(m_iters) == std::get<0>(other.m_iters);
      }

      std::ptrdiff_t distance_to(const tuple_raster_iterator& other) const {
        if constexpr (N == 0)
        {
          return 0;
        }
        return std::get<0>(other.m_iters) - std::get<0>(m_iters);
      }
      std::tuple<I...> m_iters;
    };

    template<class... R> 
    class tuple_raster_view
    {
      static const std::size_t N = sizeof...(R);

    public:
      using sub_raster_type = tuple_raster_view<typename traits<R>::sub_raster...>;
   
      tuple_raster_view() = default;

      tuple_raster_view(const R& ...r) 
        : m_rasters(r...)
      { }

      auto begin() 
      {
        return std::apply([](auto&&... rasters) {return tuple_raster_iterator(rasters.begin()...); }, m_rasters);
      }

      auto end() 
      {
        return std::apply([](auto&&... rasters) {return tuple_raster_iterator(rasters.end()...); }, m_rasters);
      }

      int rows() const
      {
        if  constexpr (N == 0) return 0;
        return std::get<0>(m_rasters).rows();
      }

      int cols() const
      {
        if constexpr (N == 0) return 0;
        return std::get<0>(m_rasters).cols();
      }

      int size() const
      {
        if  constexpr (N == 0) return 0;
        return std::get<0>(m_rasters).size();
      }

      auto sub_raster(int start_row, int start_col, int rows, int cols) const
      {
        return sub_raster_type{ std::apply([](auto&... rasters) {(rasters.sub_raster(start_row, start_col, rows, cols), ...); }, m_rasters) };
      }

    private:
      std::tuple<R...> m_rasters;
    };

    template<class... R> // requires these to be RasterViews
    tuple_raster_view<R...> raster_tuple(const R&... r)
    {
      return tuple_raster_view<R...>(r...);
    }
  }
}
