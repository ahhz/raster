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

#include <pronto/raster/access_type.h>
#include <pronto/raster/iterator_facade.h>
#include <pronto/raster/raster.h>
#include <pronto/raster/reference_proxy.h>
#include <pronto/raster/traits.h>
#include <any>
#include <cassert>
#include <functional>
#include <type_traits>

namespace pronto {
  namespace raster {

    template<class T, access AccessType = access::read_write>
    class type_erased_reference : public proxy_reference<type_erased_reference<T, AccessType>, T>
    {
      static const bool is_mutable = AccessType != access::read_only;

    public:
      type_erased_reference() = default;
      type_erased_reference(const type_erased_reference&) = default;
      type_erased_reference(type_erased_reference&&) = default;
      ~type_erased_reference() = default;

      operator T() const
      {
        return m_get();
      }

      const type_erased_reference& operator=(const T& value) const
        //     requires is_mutable
      {
        m_put(value);
        return *this;
      }

      std::function<T()> m_get;
      std::function<void(T)> m_put;
    };

    template<class T, access AccessType, class Proxy>
    auto erase_reference_type(Proxy p)
    {
      type_erased_reference<T, AccessType> ref;
      ref.m_get = [p]() {return static_cast<T>(p); };
      if constexpr (AccessType != access::read_only && !std::is_same_v<Proxy, T>) {
        ref.m_put = [p](const T& v) {
          p = v;
        };
      }
      else {
        ref.m_put = [](const T&) {assert(false); };
      }
      return ref;
    }

    template<class T, iteration_type IterationType = iteration_type::multi_pass, access AccessType = access::read_write>
    class type_erased_raster_iterator : public iterator_facade<type_erased_raster_iterator<T, IterationType, AccessType> >
    {

    public:
      using value_type = T;
      static const bool is_mutable = AccessType != access::read_only;
      static const bool is_single_pass = IterationType == iteration_type::single_pass;
      type_erased_raster_iterator() = default;
      type_erased_raster_iterator(const  type_erased_raster_iterator& iter) = default;
      type_erased_raster_iterator(type_erased_raster_iterator&& iter) = default;
      type_erased_raster_iterator& operator=(const type_erased_raster_iterator&) = default;
      type_erased_raster_iterator& operator=(type_erased_raster_iterator&&) = default;
      ~type_erased_raster_iterator() = default;

      template<class Iter>
      type_erased_raster_iterator(const Iter& iter)
      {
        using iter_type = Iter;// std::remove_cv_t<Iter>;
        m_any_iter = std::make_any<iter_type>(iter);
        m_increment = [](std::any& it) {
          iter_type& i = std::any_cast<iter_type&>(it);
          ++i;
        };
        m_decrement = [](std::any& it) {--std::any_cast<iter_type&>(it); };
        m_advance = [](std::any& it, std::ptrdiff_t offset) {std::any_cast<iter_type&>(it) += offset; };
        m_equal_to = [](const std::any& it, const type_erased_raster_iterator& other)
        {
          return std::any_cast<const iter_type&>(it) == std::any_cast<const iter_type&>(other.m_any_iter);
        };

        m_distance_to = [](const std::any& it, const type_erased_raster_iterator& other)
        {
          return std::any_cast<const iter_type&>(other.m_any_iter) - std::any_cast<const iter_type&>(it);
        };

        m_dereference = [](const std::any& it) {return erase_reference_type<T, AccessType>(*(std::any_cast<const iter_type&>(it))); };

      }

      type_erased_reference<T, AccessType> dereference() const { return m_dereference(m_any_iter); };
      void increment() { m_increment(m_any_iter); }
      void decrement() { m_decrement(m_any_iter); }
      void advance(std::ptrdiff_t offset) { m_advance(m_any_iter, offset); }
      bool equal_to(const type_erased_raster_iterator& other) const { return m_equal_to(m_any_iter, other); };
      std::ptrdiff_t distance_to(const type_erased_raster_iterator& other) const { return m_distance_to(m_any_iter, other); };

    private:
      std::function<type_erased_reference<T, AccessType>(const std::any&)> m_dereference;
      std::function<void(std::any&)> m_increment;
      std::function<void(std::any&)> m_decrement;
      std::function<void(std::any&, std::ptrdiff_t)> m_advance;
      std::function<bool(const std::any&, const type_erased_raster_iterator&)> m_equal_to;
      std::function<std::ptrdiff_t(const std::any&, const type_erased_raster_iterator&)> m_distance_to;

      std::any m_any_iter;
    };

    template<class T, iteration_type IterationType = iteration_type::multi_pass, access AccessType = access::read_write>
    class type_erased_raster : public std::ranges::view_interface<type_erased_raster<T, IterationType, AccessType> >
    {
    public:
      static const bool is_mutable = AccessType != access::read_only;
      static const bool is_single_pass = IterationType == iteration_type::single_pass;
      static const bool is_type_erased = true;

      using iterator = typename type_erased_raster_iterator<T, IterationType, AccessType>;

      type_erased_raster() = default;
      type_erased_raster(const type_erased_raster&) = default;
      type_erased_raster(type_erased_raster&&) = default;
      type_erased_raster& operator=(const type_erased_raster&) = default;
      type_erased_raster& operator=(type_erased_raster&&) = default;

      ~type_erased_raster() {};


      template<class Raster>
      type_erased_raster(const Raster& r) {
        using raster_type = Raster;// std::remove_cv_t<Raster>;
        m_raster = std::make_any<raster_type>(r);
        m_begin = [](const std::any& raster) {return iterator(std::any_cast<const raster_type&>(raster).begin()); };
        m_end = [](const std::any& raster) {return iterator(std::any_cast<const raster_type&>(raster).end()); };
        m_rows = [](const std::any& raster) {return std::any_cast<const raster_type&>(raster).rows(); };
        m_cols = [](const std::any& raster) {return std::any_cast<const raster_type&>(raster).cols(); };
        m_size = [](const std::any& raster) {return std::any_cast<const raster_type&>(raster).size(); };
        m_sub_raster = [](const std::any& raster, int a, int b, int c, int d)
        {
          return type_erased_raster<T>(std::any_cast<const raster_type&>(raster).sub_raster(a, b, c, d));
        };
      };

      iterator begin() const { return m_begin(m_raster); }
      iterator end()   const { return m_end(m_raster); }
      int rows()       const { return m_rows(m_raster); }
      int cols()       const { return m_cols(m_raster); }
      int size()       const { return m_size(m_raster); }
      type_erased_raster sub_raster(int a, int b, int c, int d) const { return m_sub_raster(m_raster, a, b, c, d); }

    private:
      std::function<iterator(const std::any&)> m_begin;
      std::function<iterator(const std::any&)> m_end;
      std::function<int(const std::any&)> m_rows;
      std::function<int(const std::any&)> m_cols;
      std::function<int(const std::any&)> m_size;
      std::function <type_erased_raster(const std::any&, int, int, int, int)> m_sub_raster;

      std::any m_raster;

    };

    template<class R>
    auto erase_raster_type(R r)
    {
      const static access access_type = std::ranges::output_range<R, std::ranges::range_value_t<R>>
        && !std::is_same_v<std::ranges::range_reference_t<R>, std::ranges::range_value_t<R> >
        ? access::read_write
        : access::read_only;

      const static iteration_type i_type = std::ranges::forward_range<R> ? iteration_type::multi_pass : iteration_type::single_pass;

      return type_erased_raster<std::ranges::range_value_t<R>, i_type, access_type>(r);
    }
  }
}
 