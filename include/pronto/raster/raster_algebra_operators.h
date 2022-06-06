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

#include <pronto/raster/optional.h>
#include <pronto/raster/raster_algebra_wrapper.h>
#include <pronto/raster/traits.h>

#include <functional>

#pragma warning( push )
#pragma warning( disable : 4244 )  // Suppressing warning related to casting,
#pragma warning( disable : 4267 )  // these are inherent to any_blind_raster

namespace pronto {
  namespace raster {
    template<class F>
    struct blind_transforming_function_struct
    {
      blind_transforming_function_struct(F f) : m_f(f)
      {
      }
      F m_f;

      template<class... Args>
      auto operator()(Args&&... args)->decltype(make_any_blind_raster(
        transform(m_f, std::forward<Args>(args)...)))
      {
        return make_any_blind_raster(
          transform(m_f, std::forward<Args>(args)...));
      }

    };

    template<class F>
    blind_transforming_function_struct<F> blind_transforming_function(F f)
    {
      return  blind_transforming_function_struct<F>(f);
    }

    template<class F, class A, class B>
    struct applicator
    {
      //using type = transform_raster_view<F, A, B>;

      static auto f(F function, A a, B b)
      {
        return transform(function, a, b);
      }
    };

    template<class F, class A, class B>
    using return_type
      = decltype(std::declval<F>()(std::declval<A>(), std::declval<B>()));


    template<class F, class A, class U>
    struct applicator<F, A, any_raster<U> >
    {
      using T = typename traits<A>::value_type;
      using type = any_raster< return_type<F, T, U> >;

      static type f(F function, A a, any_raster<U> b)
      {
        return make_any_raster(transform(function, a, b));
      }
    };

    template<class F, class T, class B>
    struct applicator<F, any_raster<T>, B >
    {
      using U = typename traits<B>::value_type;
      using type = any_raster< return_type<F, T, U> >;

      static type f(F function, any_raster<T> a, B b)
      {
        return make_any_raster(transform(function, a, b));
      }
    };

    template<class F, class T, class U>
    struct applicator<F, any_raster<T>, any_raster<U> >
    {
      using type = any_raster< return_type<F, T, U> >;

      static type f(F function, any_raster<T> a, any_raster<U> b)
      {
        return make_any_raster(transform(function, a, b));
      }
    };

    template<class F, class A>
    struct applicator<F, A, any_blind_raster >
    {
      static any_blind_raster f(F f, A a, any_blind_raster b)
      {
        auto f_transform = blind_transforming_function(f);
        return blind_function_2(f_transform, a, b);
      }
    };

    template<class F, class B>
    struct applicator<F, any_blind_raster, B >
    {
      static any_blind_raster f(F f, any_blind_raster a, B b)
      {
        auto f_transform = blind_transforming_function(f);
        return blind_function_2(f_transform, a, b);
      }
    };

    template<class F>
    struct applicator<F, any_blind_raster, any_blind_raster>
    {
      static any_blind_raster f(F f, any_blind_raster a, any_blind_raster b)
      {
        auto f_transform = blind_transforming_function(f);
        return blind_function_2(f_transform, a, b);
      }
    };

    template<class F, class T, class U>
    struct raster_algebra_apply;
    //{
    //  static_assert(false, "T or U must be a raster_algebra_wrapper");
    //};

    template<class F, class T, class U>
    struct raster_algebra_apply<F, raster_algebra_wrapper<T>, U>
    {
      using r1 = T;
      using r2 = uniform_raster_view<U>;
      using fun = applicator< F, r1, r2 >;
      using type = decltype(fun::f(std::declval<F>(),
        std::declval<r1>(), std::declval<r2>()));
      using wrapped = raster_algebra_wrapper<type>;

      static wrapped f(F function, raster_algebra_wrapper<T> t, U u)
      {
        return raster_algebra_wrap(
          fun::f(function, t.unwrap(), uniform(t.rows(), t.cols(), u)));
      };
    };

    template<class F, class T, class U>
    struct raster_algebra_apply<F, T, raster_algebra_wrapper<U> >
    {
      using r1 = uniform_raster_view<T>;
      using r2 = U;
      using fun = applicator< F, r1, r2 >;
      using type = decltype(fun::f(std::declval<F>(),
        std::declval<r1>(), std::declval<r2>()));
      using wrapped = raster_algebra_wrapper<type>;

      static wrapped f(F function, T t, raster_algebra_wrapper<U> u)
      {
        return raster_algebra_wrap(
          fun::f(function, uniform(u.rows(), u.cols(), t), u.unwrap()));
      };
    };

    template<class F, class T, class U>
    struct raster_algebra_apply
      <F, raster_algebra_wrapper<T>, raster_algebra_wrapper<U> >
    {
      using r1 = T;
      using r2 = U;
      using fun = applicator< F, r1, r2 >;
      using type = decltype(fun::f(std::declval<F>(),
        std::declval<r1>(), std::declval<r2>()));
      using wrapped = raster_algebra_wrapper<type>;

      static wrapped f(F function, raster_algebra_wrapper<T> t
        , raster_algebra_wrapper<U> u)
      {
        return raster_algebra_wrap(fun::f(function, t.unwrap(), u.unwrap()));
      };
    };

    // Binary operators
    // Requires T or U is raster_algebra_wrapper
    template<class F, class T, class U>
    auto raster_algebra(F function, T t, U u)
      -> decltype(raster_algebra_apply<F, T, U>::f(function, t, u))
    {
      return raster_algebra_apply<F, T, U>::f(function, t, u);
    }

    // Unary operators
    template<class F, class T>
    auto raster_algebra(F function, raster_algebra_wrapper<T> t)
      -> decltype(raster_algebra_wrap(transform(function, t.unwrap())))
    {
      return raster_algebra_wrap(transform(function, t.unwrap()));
    }

    template<class F>
    auto raster_algebra(F function, raster_algebra_wrapper<any_blind_raster> t)
      -> decltype(raster_algebra_wrap(blind_operator(function, t.unwrap())))
    {
      return
        raster_algebra_wrap(
          blind_function(
            blind_transforming_function_struct<F>(function), t.unwrap()
          ));
    }

    template<template<typename>class F, class T1, class T2 >
    struct filtered_binary_operator
    {
      using value_type_1 = recursive_optional_value_type<T1>;
      using value_type_2 = recursive_optional_value_type<T2>;

      using common_type = typename std::common_type<value_type_1, value_type_2>::type;
      using function_type = F<common_type>;
      using function_return_type = decltype(std::declval<function_type>()
        (std::declval<common_type>(), std::declval<common_type>()));

      using return_type = std::optional<function_return_type>;

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
      using common_type = typename std::common_type<T1, T2>::type;
      using function_type = F<common_type>;
      using return_type = decltype(std::declval<function_type>()
        (std::declval<common_type>(), std::declval<common_type>()));

      static return_type eval(const T1& v1, const T2& v2)
      {
        return  function_type{}(v1,v2);
      }
      static return_type eval(T1&& v1, T2&& v2)
      {
        return  function_type{}(std::move(v1), std::move(v2));
      }

    };

    template<template<typename>class F, class T1, class T2 >
    struct optional_filtered_binary_operator_helper
    {
      static const bool is_optional =
        is_optional_v<T1> || is_optional_v<T2>;
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
      typename optional_filtered_binary_operator<F, std::decay_t<T1>, std::decay_t<T2> >::return_type
        operator()(T1&& v1, T2&& v2) const
      {
        return optional_filtered_binary_operator<F, std::decay_t<T1>, std::decay_t<T2> >::eval(std::forward<T1>(v1), std::forward<T2>(v2));
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
      std::optional<decltype(F<T>{}(T{})) >
        operator()(std::optional<T>&& v) const
      {
        if (v) {
          return F<T>{}(*v);
        }
        else {
          return std::optional<T>{};
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

/*
template<class template<class> Func, class T1, class T2>
class binary_function_getter
{
  class unknown_type {};

  template<class T>
  struct type_or_value_type
  {
    using type = T;
  };

  template<class R>
  struct type_or_value_type<raster_algebra_wrapper<R> >
  {
    using type = typename traits<R>::value_type;
  };

  template<>
  struct type_or_value_type<raster_algebra_wrapper<any_blind_raster> >
  {
    using type = unknown_type;
  };

  template<class T1, class T2>
  struct common_type
  {
    using type = typename std::common_type<T1, T2>::type;
  };

  template<class T1>
  struct common_type<T1, unknown_type>
  {
    using type = unknown_type;
  };

  template<class T2>
  struct common_type<unknown_type, T2>
  {
    using type = unknown_type;
  };

  template<>
  struct common_type<unknown_type, unknown_type>
  {
    using type = unknown_type;
  };

  using value_type_1 = typename type_or_value_type<T1>::type;
  using value_type_2 = typename type_or_value_type<T2>::type;

  using common_value_type = typename common_type<value_type_1, value_type_2>::type;

  template<class CommonType>
  struct function_type_helper
  {
    using type = Func<CommonType>;
  };

  template<>
  struct function_type_helper<unknown_type>
  {
    using type = filtered_binary_f<Func>;
  };

  using function_type = typename function_type_helper<commpon_value_type>::type;


  function_type get()
  {
    return function_type{};
  }
};
*/

#define PRONTO_RASTER_RA_BINARY_OP(op, func)                           \
  namespace pronto {                                                  \
    namespace raster {                                                \
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
  } // end of PRONTO_raster_RA_BINARY_OP(op, func)

#define PRONTO_RASTER_RA_UNARY_OP(op, func)                         \
  namespace pronto {                                                   \
    namespace raster {                                             \
                                                                      \
      template<class R>                                               \
      auto operator op(raster_algebra_wrapper<R> r)                   \
      ->decltype(raster_algebra(filtered_unary_f<func>{}, r))         \
      {                                                               \
        return raster_algebra(filtered_unary_f<func>{}, r);           \
      }                                                               \
    }                                                                 \
  } // end of PRONTO_raster_RA_UNARY_OP(op, func)


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

PRONTO_RASTER_RA_BINARY_OP(+, std::plus)
PRONTO_RASTER_RA_BINARY_OP(-, std::minus)
PRONTO_RASTER_RA_BINARY_OP(/, std::divides)
PRONTO_RASTER_RA_BINARY_OP(%, my_modulus)
PRONTO_RASTER_RA_BINARY_OP(*, std::multiplies)
PRONTO_RASTER_RA_BINARY_OP(&&, std::logical_and)
PRONTO_RASTER_RA_BINARY_OP(||, std::logical_or)
PRONTO_RASTER_RA_BINARY_OP(>, std::greater)
PRONTO_RASTER_RA_BINARY_OP(>=, std::greater_equal)
PRONTO_RASTER_RA_BINARY_OP(<, std::less)
PRONTO_RASTER_RA_BINARY_OP(<= , std::less_equal)
PRONTO_RASTER_RA_BINARY_OP(== , std::equal_to)
PRONTO_RASTER_RA_BINARY_OP(!= , std::not_equal_to)

PRONTO_RASTER_RA_UNARY_OP(-, std::negate)
PRONTO_RASTER_RA_UNARY_OP(!, std::logical_not)
// Unary plus? -> bit pointless

#pragma warning( pop )
