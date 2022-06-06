//
//=======================================================================
// Copyright 2016-2022
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once
#include <optional>

namespace pronto {
  namespace raster {
  
    template<class T>
    static const bool is_optional_v = false;

    template<class T>
    static const bool is_optional_v<std::optional<T> > = true;


    template<class T>
    auto recursive_get_value(const T& v)
    {
      if constexpr (is_optional_v<T>) {
        return recursive_get_value(*v);
      }
      else {
        return v;
      }
    }
    template<class T>
    using recursive_optional_value_type = decltype(recursive_get_value(std::declval<T>()));
    
    template<class T>
    bool recursive_is_initialized(const T& v)
    {
      if constexpr (is_optional_v<T>) {
        return v && recursive_is_initialized(*v);
      }
      else {
        return true;
      }
    }
           
    template<class F>
    struct optional_filtered_function
    {

      optional_filtered_function(F f = F{}) : m_f(f)
      {}

      optional_filtered_function() = default;
      optional_filtered_function(const optional_filtered_function&) = default;
      optional_filtered_function(optional_filtered_function&&) = default;
      optional_filtered_function& operator=(const optional_filtered_function&) = default;
      optional_filtered_function& operator=(optional_filtered_function&&) = default;

     
      template<class... Args>
      auto operator()(Args&&... args) 
      {
        using value_type = decltype(std::declval<F>()(recursive_get_value(std::forward<Args>(args))...));

        if constexpr ((... || is_optional_v<Args>)) {
          if ((... && recursive_is_initialized(args)))
          {
            return std::optional<value_type>(m_f(recursive_get_value(std::forward<Args>(args))...));
          }
          else {
            return std::optional<value_type>();
          }
        }
        else {
          return m_f(std::forward<Args>(args)...);
        }
      }

      F m_f;
    };


    // if any of the arguments is an optional, return an optional, otherwise return plain. 
    // is any of the arguments is not initialized, return an uninitialized optional, otherwise return .
    template<class F>
    auto optionalize_function(F f)
    {
      return optional_filtered_function<F>(f);
    }
  }
}

