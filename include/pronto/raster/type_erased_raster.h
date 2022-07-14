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
    namespace detail {
      template<class Iter> void increment(std::any& iter)
      {
        ++std::any_cast<Iter&>(iter);
      }

      template<class Iter> void decrement(std::any& iter)
      {
        --std::any_cast<Iter&>(iter);
      }

      template<class Iter> void advance(std::any& iter, std::ptrdiff_t diff)
      {
        std::any_cast<Iter&>(iter) += diff;
      }

      template<class Iter> bool equal_to(const std::any& iter_a, const std::any& iter_b)
      {
        return std::any_cast<const Iter&>(iter_b) == std::any_cast<const Iter&>(iter_a);
      }

      template<class Iter> std::ptrdiff_t distance_to(const std::any& iter_a, const std::any& iter_b)
      {
        return std::any_cast<const Iter&>(iter_b) - std::any_cast<const Iter&>(iter_a);
      }

      template<class Iter> auto get(const std::any& iter)
      {
        using value_type = typename std::iterator_traits<Iter>::value_type;
        return static_cast<value_type>(*std::any_cast<const Iter&>(iter));
      }

      template<class Iter> void put(const std::any& iter, const typename std::iterator_traits<Iter>::value_type& v)
      {
        using value_type = typename std::iterator_traits<Iter>::value_type;
        auto ref = *std::any_cast<const Iter&>(iter);
        ref = v;
      }
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
        m_any_iter = std::make_any<Iter>(iter);
        m_increment = detail::increment<Iter>;
        m_decrement = detail::decrement<Iter>;
        m_advance = detail::advance<Iter>;
        m_equal_to = detail::equal_to<Iter>;
        m_distance_to = detail::distance_to<Iter>;
        m_get = detail::get<Iter>;
        m_put = detail::put<Iter>;

      //  m_dereference = [](const std::any& it) {return erase_reference_type<T, AccessType>(*(std::any_cast<const iter_type&>(it))); };

      }

      //type_erased_reference<T, AccessType> dereference() const { return m_dereference(m_any_iter); };

      void increment() 
      { 
        m_increment(m_any_iter); 
      }
      
      void decrement() 
      { 
        m_decrement(m_any_iter); 
      }
      
      void advance(std::ptrdiff_t offset) 
      { 
        m_advance(m_any_iter, offset); 
      }

      bool equal_to(const type_erased_raster_iterator& other) const 
      { 
        return m_equal_to(m_any_iter, other.m_any_iter); 
      }
      
      std::ptrdiff_t distance_to(const type_erased_raster_iterator& other) const
      {
        return m_distance_to(m_any_iter, other.m_any_iter);
      }

      T get() const
      {
        return m_get(m_any_iter);
      }

      void put(const T& v) const
      {
        return m_put(m_any_iter, v);
      }

      auto dereference() const {
        if constexpr (is_mutable)
        {
          if constexpr (is_single_pass)
          {
            return put_get_proxy_reference<const type_erased_raster_iterator&>(*this);
          }
          else {
            return put_get_proxy_reference<type_erased_raster_iterator>(*this);
          }
        }
        else {
          return get();
        }
      }
    private:
     // std::function<type_erased_reference<T, AccessType>(const std::any&)> m_dereference;
      
      void(*m_increment)(std::any&);
      void(*m_decrement)(std::any&);
      void(*m_advance)(std::any&, std::ptrdiff_t);
      bool(*m_equal_to)(const std::any&, const std::any&);
      std::ptrdiff_t(*m_distance_to)(const std::any&, const std::any&);
      T(*m_get)(const std::any&);
      void(*m_put)(const std::any&, const T&);

      const std::any& get_iter() const
      {
        return m_any_iter;
      }

      std::any m_any_iter;
    };

    template<class T, iteration_type IterationType = iteration_type::multi_pass, access AccessType = access::read_write>
    class type_erased_raster : public std::ranges::view_interface<type_erased_raster<T, IterationType, AccessType> >
    {
    public:
      static const bool is_mutable = AccessType != access::read_only;
      static const bool is_single_pass = IterationType == iteration_type::single_pass;
     
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
      constexpr auto iter_type = std::ranges::forward_range<R> ? iteration_type::multi_pass : iteration_type::single_pass;
      constexpr auto writeable = std::ranges::output_range < R, std::ranges::range_value_t<R> >;
      constexpr auto ref_equals_value = std::is_same_v< std::ranges::range_reference_t<R>, std::ranges::range_value_t<R>>;
      constexpr auto acc_type = writeable && !ref_equals_value ? access::read_write : access::read_only;


      return type_erased_raster<std::ranges::range_value_t<R>, iter_type, acc_type>(r);
    }
  }
}
 