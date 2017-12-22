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

#include <blink/raster/any_blind_raster.h>

#include <functional>
#include <tuple>
#include <vector>
 
#pragma warning( push )  
#pragma warning( disable : 4244 )  // Suppressing warning related to casting,
#pragma warning( disable : 4267 )  // these are inherent to any_blind_raster

namespace blink {
  namespace raster {

    template<class F>
    using blind_function_return_type =
      decltype(std::declval<F>()(any_raster<int>{}));

    template<class T, class F>
    blind_function_return_type<F>
      blind_function_typed(F f, any_blind_raster r)
    {
      return f(r.get_by_type<T>());
    }

    template<class, class>  struct make_functions;
    template<class F, class... T>
    struct make_functions<F, std::tuple<T...>>
    {
      using return_type = blind_function_return_type<F>;
      std::vector<std::function<return_type(F, any_blind_raster) > >operator()
        (F f, any_blind_raster r)
      {
        return std::vector<std::function<return_type(F, any_blind_raster)> >{
          std::bind(blind_function_typed<T, F>
            , std::placeholders::_1, std::placeholders::_2)...};
      }
    };

    // Create a vector of functions for all supported types, and then apply on 
    // the actual type (which is only known at runtime)
    template<class F>
    blind_function_return_type<F> blind_function(F f, any_blind_raster r)
    {
      using return_type = blind_function_return_type<F>;
      using function_type = std::function<return_type(F, any_blind_raster)>;
      using function_maker = make_functions<F, blind_data_types>;
      std::vector<function_type> functions = function_maker{}(f, r);
      return functions[r.index()](f, r);
    }

    // Must use the applicator pattern, because the return types of the 
    // unused overloads cannot be derived. 
    // This can be overcome by making traits<Raster> more robust
    template<class F, class A, class B>
    struct blind_function_2_applicator;

    using std::placeholders::_1;

    template<class F, class B>
    struct blind_function_2_applicator<F, any_blind_raster, B>
    {
      template<class F, class T>
      auto operator()(F f, any_blind_raster r, T v)
        ->decltype(blind_function(std::bind(f, _1, v), r))
      {
        return blind_function(std::bind(f, _1, v), r);
      }
    };

    template<class F, class A>
    struct blind_function_2_applicator<F, A, any_blind_raster>
    {
      template<class F, class T>
      auto operator()(F f, T v, any_blind_raster r)
        ->decltype(blind_function(std::bind(f, v, _1), r))
      {
        return blind_function(std::bind(f, v, _1), r);
      }
    };

    template<class F>
    struct blind_function_2_helper
    {
      blind_function_2_helper(F f, any_blind_raster first_arg)
        : m_f(f), m_first_arg(first_arg)
      {}
      template<class T>  auto operator()(any_raster<T> second_arg)
        ->decltype(blind_function(std::bind(m_f, _1, second_arg), m_first_arg))
      {
        return blind_function(std::bind(m_f, _1, second_arg), m_first_arg);
      }

      any_blind_raster m_first_arg;
      F m_f;
    };

    template<class F>
    struct blind_function_2_applicator<F, any_blind_raster, any_blind_raster>
    {
      auto operator()(F f, any_blind_raster r1, any_blind_raster r2)
        ->decltype(blind_function(blind_function_2_helper<F>{f, r1}, r2))
      {
        return blind_function(blind_function_2_helper<F>{f, r1}, r2);
      }
    };

    template<class F, class A, class B>
    auto blind_function_2(F f, A a, B b)
      ->decltype(blind_function_2_applicator<F, A, B>{}(f, a, b))
    {
      return blind_function_2_applicator<F, A, B>{}(f, a, b);
    }
  }
}
#pragma warning( pop )