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
#include <blink/raster/raster_algebra_wrapper.h>
#include <blink/raster/traits.h>

#include <functional>
#pragma warning( push )  
#pragma warning( disable : 4244 )  // Suppressing warning related to casting,
#pragma warning( disable : 4267 )  // these are inherent to any_blind_raster
namespace blink {
  namespace raster {

    using optional_dimension = optional<std::pair<int, int> >;

    optional_dimension get_dimension()
    {
      return optional_dimension{};
    };
   
    template<class R, class... Rest>
    optional_dimension get_dimension(const raster_algebra_wrapper<R>& r, const Rest&...)
    {
      return optional_dimension{ std::make_pair(r.rows(), r.cols()) };
    }

    template<class T, class... Rest>
    optional_dimension get_dimension(const T&, const Rest&... rest)
    {
      return get_dimension(rest...);
    }

    bool check_same_dimension()
    {
      return true;
    };

    template<class R, class... Rest>
    bool check_same_dimension(const raster_algebra_wrapper<R>& r
      , const Rest&... rest)
    {
      optional_dimension dim = get_dimension(rest...);
      if (dim) {
        return dim == get_dimension(r) && check_same_dimension(rest...);
      }
      else return true;
    }

    template<class T, class... Rest>
    bool check_same_dimension(const T&, const Rest&... rest)
    {
      return check_same_dimension(rest...);
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
    struct check_contains_wrapped_raster;

    template <>
    struct  check_contains_wrapped_raster<>
    {
      static const bool value = false;
      using type = std::false_type;
    };

    template <typename T, typename... Rest>
    struct  check_contains_wrapped_raster<raster_algebra_wrapper<T>, Rest...>
    {
      static const bool value = true;
      using type = std::true_type;
    };
 
    template <typename T, typename... Rest>
    struct  check_contains_wrapped_raster<T, Rest...>
    {
      static const bool value = check_contains_wrapped_raster<Rest...>::value;
      using type = typename check_contains_wrapped_raster<Rest...>::type;
    };

    template <typename... Args>
    using contains_wrapped_raster =
      typename check_contains_wrapped_raster<Args...>::type;

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
      ->decltype(as_raster_t<T>::f(raster_or_value, rows, cols))
    {
      return as_raster_t<T>::f(raster_or_value, rows, cols);
    }

    // If it is a raster, apply the function on the rasters
    template<class F, class... Args>
    auto raster_algebra_transform_helper(std::true_type, F function, Args&&... args)
      ->decltype(raster_algebra_wrap(transform(function, 
        as_raster(std::forward<Args>(args), 0, 0)...)))
    {
       
      const optional_dimension d = get_dimension(std::forward<Args>(args)...);
      const int rows = d->first;
      const int cols = d->second;
     
      assert(d); 
      assert(check_same_dimension(std::forward<Args>(args)...));
      
      return raster_algebra_wrap(transform(function, 
        as_raster(std::forward<Args>(args), rows, cols)...));
    }

    // If it is not a raster, apply the function on the values
    template<class F, class... Args>
    auto raster_algebra_transform_helper(std::false_type, F function, Args&&... args)
      -> decltype(function(std::forward<Args>(args)...))
    {
      return function(std::forward<Args>(args)...);
    }
        
    template< class F, class... Args>
    auto 
      raster_algebra_transform(F function, Args&&... args)
      -> decltype(raster_algebra_transform_helper(
        contains_wrapped_raster<typename std::decay<Args>::type...>{},
        function, std::forward<Args>(args)...))
    {
      return raster_algebra_transform_helper(
        contains_wrapped_raster<typename std::decay<Args>::type...>{},
        function, std::forward<Args>(args)...);
    }
  }
#pragma warning( pop )  
}
