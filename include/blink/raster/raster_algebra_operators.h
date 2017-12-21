//
//=======================================================================
// Copyright 2016-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// MACRO to provide operator overloads for range_algebra_wrappers
//
// Overload raster_algebra operator "op" and ties it to function "func". 

#pragma once

#include <blink/raster/raster_algebra.h>
#include <blink/raster/traits.h>
#include <blink/raster/optional.h>

#include <functional>

#pragma warning( push )  
#pragma warning( disable : 4244 )  // Suppressing warning related to casting,
#pragma warning( disable : 4267 )  // these are inherent to any_blind_raster

namespace blink {
  namespace raster {
    template<template<typename>class F, class T1, class T2 >
    struct filtered_binary_operator
    {
      using value_type_1 = recursive_optional_value_type<T1>;
      using value_type_2 = recursive_optional_value_type<T2>;

      using common_type = typename std::common_type<value_type_1, value_type_2>::type;
      using function_type = F<common_type>;
      using function_return_type = decltype(std::declval<function_type>()
        (std::declval<common_type>(), std::declval<common_type>()));

      using return_type = optional<function_return_type>;

      static return_type eval(const T1& v1, const T2& v2) 
      {
        if (recursive_is_initialized(v1) && recursive_is_initialized(v2)) {
          return function_type{}(recursive_get_value(v1), 
            recursive_get_value(v2));
        }
        else {
          return return_type{};
        }
      }
    };

    template<template<typename>class F, class T1, class T2 >
    struct unfiltered_binary_operator
    {
      using common_type = typename std::common_type<T1, T1>::type;
      using function_type = F<common_type>;
      using return_type = decltype(std::declval<function_type>()
        (std::declval<common_type>(), std::declval<common_type>()));

      static return_type eval(const T1& v1, const T2& v2) 
      {
        return  function_type{}(v1,v2);
      }
    };

    template<template<typename>class F, class T1, class T2 >
    struct optional_filtered_binary_operator_helper
    {
      static const bool is_optional =
        is_optional<T1>::value || is_optional<T2>::value;
      using type = std::conditional_t< is_optional
        , filtered_binary_operator<F, T1, T2>
        , unfiltered_binary_operator<F, T1, T2> >;

    };

    template<template<typename>class F, class T1, class T2 >
    using  optional_filtered_binary_operator =
      typename optional_filtered_binary_operator_helper<F, T1, T2>::type;

    template<template<typename> class F >
    struct filtered_binary_f
    {
      template<class T1, class T2>
      typename optional_filtered_binary_operator<F, T1, T2>::return_type
        operator()(const T1& v1, const T2& v2) const
      {
        return optional_filtered_binary_operator<F, T1, T2>::eval(v1, v2); 
      }
    };
    
    template<template<class> class F >
    struct filtered_unary_f
    {
      template<class T>
      decltype(F<T>{}(T{})) operator()(const T& v)const
      {
        return F<T>{}(v);
      }

      template<class T>
      optional<decltype(F<T>{}(T{})) >
        operator()(optional<T>&& v) const
      {
        if (v) {
          return F<T>{}(*v);
        }
        else {
          return optional<T>{};
        }
      }
     
    };

    template<class T>
    struct my_modulus
    {
      using type = typename
        std::conditional<std::is_integral<T>::value, T, int>::type;
      
      auto operator()(const T& v1, const T& v2)const->decltype(type{} % type{})
      {
        assert(std::is_integral<T>::value); // TODO: should emit a warning
        return static_cast<type>(v1) % static_cast<type>(v2);
      }
    };

  
  }
}

#define BLINK_raster_RA_BINARY_OP(op, func)                        \
  namespace blink {                                                   \
    namespace raster {                                             \
                                                                      \
      template<class R, class T>                                      \
      auto operator op(raster_algebra_wrapper<R> r, const T& v)       \
        ->decltype (raster_algebra(filtered_binary_f<func>{}, r, v))  \
      {                                                               \
        return raster_algebra(filtered_binary_f<func>{}, r, v);       \
      }                                                               \
                                                                      \
      template<class T, class R>                                      \
      auto operator op(const T& v, raster_algebra_wrapper<R> r)       \
        ->decltype (raster_algebra(filtered_binary_f<func>{}, v, r))  \
      {                                                               \
        return raster_algebra(filtered_binary_f<func>{}, v, r);       \
      }                                                               \
                                                                      \
      template<class R1, class R2>                                    \
      auto operator op(raster_algebra_wrapper<R1> r1                  \
        , raster_algebra_wrapper<R2> r2)                              \
        ->decltype (raster_algebra(filtered_binary_f<func>{}, r1, r2))\
      {                                                               \
        return raster_algebra(filtered_binary_f<func>{}, r1, r2);     \
      }                                                               \
    }                                                                 \
  } // end of BLINK_raster_RA_BINARY_OP(op, func)

#define BLINK_raster_RA_UNARY_OP(op, func)                         \
  namespace blink {                                                   \
    namespace raster {                                             \
                                                                      \
      template<class R>                                               \
      auto operator op(raster_algebra_wrapper<R> r)                   \
      ->decltype(raster_algebra(filtered_unary_f<func>{}, r))         \
      {                                                               \
        return raster_algebra(filtered_unary_f<func>{}, r);           \
      }                                                               \
    }                                                                 \
  } // end of BLINK_raster_RA_UNARY_OP(op, func)


/*
template<template<class> class F, class V1, class V2>
struct suppress_cast_warnings_binary;

template<template<class> class F, class T, class U>
struct suppress_cast_warnings_binary<F, raster_algebra_wrapper<T>,  V2>
{
  using V1 = traits<T>::value_type;
  using type = std::common_type<V1, V2>;
  using function = F<type>;
  auto operator()(V1 v1, V2 v2)->decltype(F<type>(type{}, type{}))
  {
    return F<type>{}(static_cast<type>(v1), static_cast<type>(v2));
  }
};
*/

BLINK_raster_RA_BINARY_OP(+, std::plus) 
BLINK_raster_RA_BINARY_OP(-, std::minus)
BLINK_raster_RA_BINARY_OP(/, std::divides)
BLINK_raster_RA_BINARY_OP(%, my_modulus) 
BLINK_raster_RA_BINARY_OP(*, std::multiplies)
BLINK_raster_RA_BINARY_OP(&&, std::logical_and)
BLINK_raster_RA_BINARY_OP(||, std::logical_or)
BLINK_raster_RA_BINARY_OP(>, std::greater)
BLINK_raster_RA_BINARY_OP(>=, std::greater_equal)
BLINK_raster_RA_BINARY_OP(<, std::less)
BLINK_raster_RA_BINARY_OP(<= , std::less_equal)
BLINK_raster_RA_BINARY_OP(== , std::equal_to)
BLINK_raster_RA_BINARY_OP(!= , std::not_equal_to)

BLINK_raster_RA_UNARY_OP(-, std::negate)
BLINK_raster_RA_UNARY_OP(!, std::logical_not)
// Unary plus? -> bit pointless

#pragma warning( pop )  