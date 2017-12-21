//
//=======================================================================
// Copyright 2016-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once

#include <blink/raster/optional.h>
#include <blink/raster/raster_algebra.h>
#include <blink/raster/traits.h>

#include <functional>
#pragma warning( push )  
#pragma warning( disable : 4244 )  // Suppressing warning related to casting,
#pragma warning( disable : 4267 )  // these are inherent to any_blind_raster
namespace blink {
  namespace raster {

    using dim_t = optional<std::pair<int, int> >;

    template<typename T>
    dim_t get_dim_single(const raster_algebra_wrapper<T>& r)
    {
      return dim_t{ std::make_pair(r.rows(), r.cols() )};
    }

    template<typename T>
    dim_t get_dim_single(const T&)
    {
      return dim_t{};
    }

    template<class T, class...More>
    dim_t get_dim_multi(const dim_t& dimension, const T& entry, const More&... more)
    {
      dim_t add = get_dim_single(entry);

      if (dimension) {
        assert(!add || (dimension == add)); // TODO emit warning?
        return get_dim_multi(dimension, more...);
      }
      return get_dim_multi(add, more...);
    }

    template<class T>
    dim_t get_dim_multi(const dim_t& dimension, const T& entry)
    {
      dim_t add = get_dim_single(entry);

      if (dimension) {
        assert(!add || (dimension == add));// TODO emit warning?
        return dimension;
      }
      return add;
    }

    template<class... T>
    dim_t get_dim(const T& ...args)
    {
      return get_dim_multi(dim_t{}, args...);
    }

    template<class T>
    struct is_wrapped_raster
    {
      static const bool value = false;
    };

    template<class T>
    struct is_wrapped_raster<raster_algebra_wrapper<T>>
    {
      static const bool value = true;
    };

    using is_raster = std::true_type;
    using is_not_raster = std::false_type;

    template <typename... T>
    struct contains_wrapped_raster;

    template <>
    struct contains_wrapped_raster<> : std::false_type {};

    template <typename T, typename... Rest>
    struct contains_wrapped_raster<raster_algebra_wrapper<T>, Rest...>
      : std::true_type {};

    template <typename T, typename... Rest>
    struct contains_wrapped_raster<T, Rest...>
      : contains_wrapped_raster<Rest...> {};

    template<class T>
    struct as_raster_t
    {
      static uniform_raster_view<T> f(const T& value, int rows, int cols) 
      {
        return uniform(rows, cols, value);
      }
    };

    template<class T>
    struct as_raster_t<raster_algebra_wrapper<T>>
    {
      static T f(const raster_algebra_wrapper<T>& raster, int , int ) 
      {
        return raster.unwrap();
      }
    };

    template<class T>
    auto as_raster(const T& raster_or_value, int rows, int cols)
    {
      return as_raster_t<T>::f(raster_or_value, rows, cols);
    }

    template<class F, class... Args>
    auto raster_algebra_transform1(is_raster, F function, Args&&... args)
      ->decltype(raster_algebra_wrap(transform(function, 
        as_raster(std::forward<Args>(args), 0, 0)...)))
    {
      dim_t d = get_dim(std::forward<Args>(args)...);
      int rows = d->first;
      int cols = d->second;
      return raster_algebra_wrap(transform(function, 
        as_raster(std::forward<Args>(args), rows, cols)...));
    }

    template<class F, class... Args>
    auto raster_algebra_transform1(is_not_raster, F function, Args&&... args)
      -> decltype(function(std::forward<Args>(args)...))
    {
      return function(std::forward<Args>(args)...);
    }

    template< class F, class... Args>
    auto 
      raster_algebra_transform(F function, Args&&... args)
      -> decltype(raster_algebra_transform1(
        typename contains_wrapped_raster<std::decay_t<Args>...>::type{},
        function, std::forward<Args>(args)...))
    {
      return raster_algebra_transform1(
        typename contains_wrapped_raster<std::decay_t<Args>...>::type{},
        function, std::forward<Args>(args)...);
    }
  }
#pragma warning( pop )  
}
