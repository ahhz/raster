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

#include <pronto/raster/iterator_facade.h>
#include <pronto/raster/traits.h>

#include <iterator>
#include <utility>
#include <optional>

namespace pronto {
  namespace raster {

    // Now casting all inputs to the function to their value_type
    // this means that the proxy references will be cast, and therefore
    // all iterators are non-mutable.

    template<class View, class... I>
    class transform_raster_iterator : public iterator_facade<transform_raster_iterator<View, I...> >
    {
    private:
      static const std::size_t N = sizeof...(I);
    public:
      using value_type = typename View::value_type;
      using reference = value_type;
      const static bool is_mutable = false;
      const static bool is_single_pass = false; // should really check if any of the iterators is single_pass
      using difference_type = std::ptrdiff_t; // could get this from the first Iter
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
            
    public:
      //class iterator_facade<transform_raster_iterator<View, I...> >;
      friend iterator_facade<transform_raster_iterator<View, I...> >;
     
      reference dereference() const
      {

        auto get_value = [](const auto& iter) {
          return static_cast<std::iterator_traits<std::decay_t<decltype(iter)>>::value_type>(*iter);
       };
        auto& fun = *(m_view->m_function);
        return std::apply([&](auto& ...iter) {return fun(get_value(iter)...); }, m_iters);
      }
      void increment() 
      {
        std::apply([](auto& ...iter) { (..., ++iter); }, m_iters);
      }
 
      void decrement() 
      {
        std::apply([](auto& ...iter) { (..., --iter); }, m_iters);
      }

      void advance(difference_type n)
      {
        std::apply([n](auto& ...iter) { (..., (iter += n) ); }, m_iters);
      }

      bool equal_to(const transform_raster_iterator& b) const
      {
        return N == 0 || std::get<0>(m_iters) == std::get<0>(b.m_iters);
      }

      difference_type distance_to(const transform_raster_iterator& b) const
      {
       return N > 0 ? std::get<0>(b.m_iters) - std::get<0>(m_iters) : 0;
      }

      const View* m_view;
      std::tuple<I...> m_iters;
    };

    template<class F, class... R>
    class transform_raster_view : public std::ranges::view_interface<transform_raster_view<F, R...> >
    {
    private:
      static_assert(std::is_copy_constructible<F>::value, "because this models RasterView, use std::ref in transform function");
      static const bool is_mutable = false;
      static const bool is_single_pass = false;
      using function_type = F;

      static const std::size_t N = sizeof...(R);
      
    public:
      using value_type = decltype(std::declval<F>()(std::declval<typename traits<R>::value_type>()...));
      transform_raster_view() = default;
      transform_raster_view(const transform_raster_view&) = default;
      transform_raster_view(transform_raster_view&&) = default;
      transform_raster_view& operator=(const transform_raster_view&) = default;
      transform_raster_view& operator=(transform_raster_view&&) = default;

      template<class FF>
      transform_raster_view(FF&& f, R ...r) : m_rasters(r...), m_function(std::forward<FF>(f))
      { }

      using sub_raster_type = transform_raster_view<function_type, typename traits<R>::sub_raster...>;


      auto begin() const
      {
        return std::apply([&](auto&&... rasters) { return transform_raster_iterator{ *this, rasters.begin()... }; },m_rasters);
      }

      auto end() const
      {
        return std::apply([&](auto&&... rasters) { return transform_raster_iterator{ *this, rasters.end()... }; }, m_rasters);
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

      auto
        sub_raster(int start_row, int start_col, int rows, int cols) const
      {
        return std::apply([&](auto&&... rasters) {return sub_raster_type(*m_function, rasters.sub_raster(start_row, start_col, rows, cols)...); }, m_rasters);
      }
 
      std::tuple<R...> m_rasters;
    private:
      //friend class const_iterator;
      friend class transform_raster_iterator<transform_raster_view,
        typename traits<R>::const_iterator...>;
      mutable std::optional<function_type> m_function;
    };


    template<class F, class... R> // requires these to be RasterViews
    auto transform(F&& f, R... r)
    {
      return transform_raster_view<typename std::decay_t<F>, R...>
        (std::forward<F>(f), r...);
    }
  }
}
