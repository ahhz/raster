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

#include <pronto/raster/optional.h>
#include <pronto/raster/traits.h>
#include <pronto/raster/iterator_facade.h>

#include <functional>
#include <iterator>
#include <utility>
#include <cassert>
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
      using tuple_indices = std::make_index_sequence<N>;
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


    private:
      template<std::size_t ...S>
      void increment(std::index_sequence<S...>)
      {
        auto dummy = { (++std::get<S>(m_iters), 0)... };
      }

      template<std::size_t ...S>
      void decrement(std::index_sequence<S...>)
      {
        auto dummy = { (--std::get<S>(m_iters), 0)... };
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

    public:
      friend iterator_facade<transform_raster_iterator<View, I...> >;
     
      reference dereference() const
      {
        return dereference(tuple_indices{});
      }
      void increment() 
      {
        increment(tuple_indices{});
      }
      void decrement() 
      {
        decrement(tuple_indices{});
      }

      void advance(difference_type n)
      {
        increment_step(n, tuple_indices{});
      }

      bool equal_to(const transform_raster_iterator& b) const
      {
        return N == 0 || std::get<0>(m_iters) == std::get<0>(b.m_iters);
      }

      difference_type distance_to(const transform_raster_iterator& b) const
      {
       return N > 0 ? std::get<0>(b.m_iters) - std::get<0>(b) : 0;
      }

      const View* m_view;
      std::tuple<I...> m_iters;
    };

    template<class F, class... R>
    class transform_raster_view
    {
    private:
      static_assert(std::is_copy_constructible<F>::value, "because this models RasterView, use std::ref in transform function");
      using value_type = decltype(std::declval<F>()(std::declval<typename traits<R>::value_type>()...));

      using function_type = F;

      static const std::size_t N = sizeof...(R);
      using tuple_indices = std::make_index_sequence<N>;

    public:
      transform_raster_view() = default;
      transform_raster_view(const transform_raster_view&) = default;
      transform_raster_view(transform_raster_view&&) = default;
      transform_raster_view& operator=(const transform_raster_view&) = default;
      transform_raster_view& operator=(transform_raster_view&&) = default;

      template<class FF>
      transform_raster_view(FF&& f, R ...r) : m_rasters(r...), m_function(std::forward<FF>(f))
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

    public:

      // Not allowing the function to change values in the rasters
      template<std::size_t ...S>
      const_iterator begin(std::index_sequence<S...>) const
      {
        return const_iterator(*this, std::get<S>(m_rasters).begin()...);
      }

      template<std::size_t ...S>
      const_iterator end(std::index_sequence<S...>) const
      {
        return const_iterator(*this, std::get<S>(m_rasters).end()...);
      }

      template<std::size_t ...S>
      sub_raster_type sub_raster(std::index_sequence<S...>
        , int start_row, int start_col, int rows, int cols) const
      {
        assert(start_row >= 0 && start_col >= 0 && rows + start_row <= this->rows() && cols + start_col <= this->cols());
        return sub_raster_type
        (*m_function, std::get<S>(m_rasters).sub_raster
        (start_row, start_col, rows, cols)...);
      }

      std::tuple<R...> m_rasters;
    private:
      //friend class const_iterator;
      friend class transform_raster_iterator<transform_raster_view,
        typename traits<R>::const_iterator...>;
      mutable std::optional<function_type> m_function;
    };


    template<class T>
    using decay_t = typename std::decay<T>::type;

    template<class F>
    class reference_function
    {
    public:
      reference_function(F& f) : m_f(f)
      {}

      reference_function(const reference_function& f) = default;

      template<class...Args>
      auto operator()(Args&&... args)->decltype(std::declval<F>()(std::forward<Args>(args)...))
      {
        return m_f(std::forward<Args>(args)...);
      }
    private:
      F& m_f;
   };

    template<class F, class... R> // requires these to be RasterViews
    transform_raster_view<typename std::decay<F>::type, R...>
      transform(F&& f, R... r)
    {
      return transform_raster_view<typename std::decay<F>::type, R...>
        (std::forward<F>(f), r...);
    }

    // if the function is to be treated as a reference, wrap it.
    template<class F, class... R> // requires these to be RasterViews
    transform_raster_view<reference_function<F>, R...>
      transform(std::reference_wrapper<F> f, R... r)
    {
      return transform_raster_view<reference_function<F>, R...>
        (reference_function<F>(f), r...);
    }
  }
}
