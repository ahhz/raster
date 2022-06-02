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
#include <optional>

namespace pronto {
  namespace raster {
    /*
    template<class V>
    auto rec_get_value(V& v)
    {
      return v
    }

    template<class V>
    auto rec_get_value(std::optional<V>& v)
    {
      return rec_get_value(*v);
    }

    template<class V>
    auto rec_get_value(const std::optional<V>& v)
    {
      return rec_get_value(*v);
    }

    template<class V>
    bool rec_is_initialized(const V& v)
    {
      return true;
    }

    template<class V>
    bool rec_is_initialized(const std::optional<V>& v)
    {
      return v && rec_is_initialized(*v);
    }

    template<class V>
    bool is_initialized(const V& v)
    {
      return true;
    }

    template<class V>
    bool is_initialized(const std::optional<V>& v)
    {
      return v;
    }
    */

    namespace detail {
      template<class V>
      struct generic_optional_helper
      {
        static const bool is_optional = false;

        using value_type = typename std::remove_const<V>::type;
        static bool is_initialized(const value_type&) { return true; }
        static const value_type& get_value(const value_type& v) { return v; }
        static value_type& get_value(value_type& v) { return v; }

        using recursive_value_type = V;
        static bool recursive_is_initialized(const value_type&) { return true; }
        static const value_type& recursive_get_value(const value_type& v) { return v; }
        static value_type& recursive_get_value(value_type& v) { return v; }
      };

      template<class V>
      struct generic_optional_helper< std::optional<V> >
      {
        static const bool is_optional = true;

        using value_type = V;
        static bool is_initialized(const std::optional<V>& v) { return v; }
        static const V& get_value(const std::optional<V>& v) { return *v; }
        static V& get_value(std::optional<V>& v) { return *v; }

        using recursive_value_type = typename generic_optional_helper<V>::recursive_value_type;
        static bool recursive_is_initialized(const std::optional<V>& v)
        {
          return v && generic_optional_helper<V>::recursive_is_initialized(*v);
        }

        static const recursive_value_type& recursive_get_value(const std::optional<V>& v)
        {
          return generic_optional_helper<V>::recursive_get_value(*v);
        }

        static recursive_value_type& recursive_get_value(std::optional<V>& v)
        {
          return generic_optional_helper<V>::recursive_get_value(*v);
        }
      };
    }

    template<class V>
    using optional_value_type
      = typename detail::generic_optional_helper<V>::value_type;

    template<class V>
    using recursive_optional_value_type
      = typename detail::generic_optional_helper<V>::recursive_value_type;

    template<class V>
    using is_optional = std::integral_constant<bool
      , detail::generic_optional_helper<V>::is_optional>;

    template<class V>
    inline bool is_initialized(const V& v)
    {
      return detail::generic_optional_helper<V>::is_initialized(v);
    }

    template<class V>
    inline bool recursive_is_initialized(const V& v)
    {
      return detail::generic_optional_helper<V>::recursive_is_initialized(v);
    }

    template<class V>
    optional_value_type<V>& get_value(V& v) {
      return detail::generic_optional_helper<V>::get_value(v);
    }
    template<class V>
    const optional_value_type<V>& get_value(const V& v) {
      return detail::generic_optional_helper<V>::get_value(v);
    }

    template<class V>
    recursive_optional_value_type<V>& recursive_get_value(V& v) {
      return detail::generic_optional_helper<V>::recursive_get_value(v);
    }
    template<class V>
    const recursive_optional_value_type<V>& recursive_get_value(const V& v) {
      return detail::generic_optional_helper<V>::recursive_get_value(v);
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
      
      static bool are_all_initialized()
      {
        return true;
      }

      template<class T, class... Rest>
      static bool are_all_initialized(const T& v, const Rest&... rest)
      {
        return recursive_is_initialized(v) && are_all_initialized(rest...);
      }

      template<class... Args>
      auto operator()(Args&&... args) const
        ->std::optional<decltype(m_f(recursive_get_value(std::forward<Args>(args))...))>
      {
        if (are_all_initialized(args...))
        {
          return m_f(recursive_get_value(std::forward<Args>(args))...);
        }
        else {
          return std::nullopt;
        }

      }
      F m_f;
    };

    template<class F>
    optional_filtered_function<F> optionalize_function(F f)
    {
      return optional_filtered_function<F>(f);
    }
  }
}

