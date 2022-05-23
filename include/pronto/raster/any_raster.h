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

#include <pronto/raster/reference_proxy.h>
#include <pronto/raster/traits.h>

#include <any>
#include <cassert>
#include <type_traits>

namespace pronto {
  namespace raster {

    template<class T> class any_raster;
    template<class T> class any_raster_iterator;
    template<class T> class any_raster_const_iterator;

    template<class Iter>
    struct typed_iterator_members
    {
      using value_type = typename std::iterator_traits<Iter>::value_type;

      inline static void increment(std::any& i)
      {
        ++std::any_cast<Iter&>(i);
      }

      inline static value_type get(const std::any& i)
      {
        return *std::any_cast<const Iter&>(i);
      }

      inline static bool is_equal(const std::any& i, const std::any& j)
      {
        return any_cast<const Iter&>(i) == any_cast<const Iter&>(j);
      }

      inline static void put(const std::any& i, const value_type& v, const std::true_type&) // assignable
      {
        *any_cast<const Iter&>(i) = v;
      }

      inline static void put(const std::any& i, const value_type& v, const std::false_type&) // unassignable
      {
        assert(false); // Trying to assign
      }

      inline static void put(const std::any& i, const value_type& v)
      {
        using ref_type = typename std::iterator_traits<Iter>::reference;
        using assignable = typename std::is_assignable<ref_type, value_type>::type;
        put(i, v, assignable{});
      }
    };

    template<class Raster>
    struct typed_raster_members
    {
      using value_type = typename traits<Raster>::value_type;
      using iterator = any_raster_iterator<value_type>;
      using const_iterator = any_raster_const_iterator<value_type>;

      static iterator begin(std::any& holding)
      {
        return std::any_cast<Raster&>(holding).begin();
      }

      static iterator end(std::any& holding)
      {
        return std::any_cast<Raster&>(holding).end();
      }

      static const_iterator cbegin(const std::any& holding)
      {
        return std::any_cast<const Raster&>(holding).begin();
      }

      static const_iterator cend(const std::any& holding)
      {
        return std::any_cast<const Raster&>(holding).end();
      }

      static int rows(const std::any& holding)
      {
        return std::any_cast<const Raster&>(holding).rows();
      }

      static int cols(const std::any& holding)
      {
        return std::any_cast<const Raster&>(holding).cols();
      }

      static int size(const std::any& holding)
      {
        return std::any_cast<const Raster&>(holding).size();
      }

      static any_raster<value_type> sub_raster(const std::any& holding, int start_row, int start_col, int rows, int cols)
      {
        return any_raster<value_type>(
          std::any_cast<const Raster&>(holding).sub_raster(start_row, start_col, rows, cols));
      }
    };

    template<class T>
    class any_raster_iterator
    {
    public:
      using reference = reference_proxy<any_raster_iterator>;
      using value_type = T;
      using pointer = void;
      using difference_type = void;
      using iterator_category = std::input_iterator_tag;

      template<class Iter>
      any_raster_iterator(const Iter& i)
        : m_get(typed_iterator_members<Iter>::get)
        , m_put(typed_iterator_members<Iter>::put)
        , m_increment(typed_iterator_members<Iter>::increment)
        , m_is_equal(typed_iterator_members<Iter>::is_equal)
        , m_holding(i)
      {}

      inline any_raster_iterator& operator++()
      {
        m_increment(m_holding);
        return *this;
      }

      inline any_raster_iterator operator++(int)
      {
        any_raster_iterator copy = *this;
        m_increment(m_holding);
        return copy;
      }

      inline reference operator*() const
      {
        return reference(*this);
      }

      inline bool operator==(const any_raster_iterator& other) const
      {
        return m_is_equal(m_holding, other.m_holding);
      }

      inline bool operator!=(const any_raster_iterator& other) const
      {
        return !m_is_equal(m_holding, other.m_holding);
      }

      inline T get() const
      {
        return m_get(m_holding);
      }

      inline void put(const T& v) const
      {
        return m_put(m_holding, v);
      }

      T(*m_get)(const std::any&);
      void(*m_put)(const std::any&, const T&);
      void(*m_increment)(std::any&);
      bool(*m_is_equal)(const std::any&, const std::any&);

    public:
      std::any m_holding;
    };

    template<class T>
    class any_raster_const_iterator
    {
    public:
      using reference = T;
      using value_type = T;
      using pointer = void;
      using difference_type = void;
      using iterator_category = std::output_iterator_tag;

      template<class Iter>
      any_raster_const_iterator(const Iter& i)
        : m_get(typed_iterator_members<Iter>::get)
        , m_increment(typed_iterator_members<Iter>::increment)
        , m_is_equal(typed_iterator_members<Iter>::is_equal)
        , m_holding(i)

      {}

      inline any_raster_const_iterator& operator++()
      {
        m_increment(m_holding);
        return *this;
      }

      inline any_raster_const_iterator operator++(int)
      {
        any_raster_const_iterator copy = *this;
        m_increment(m_holding);
        return copy;
      }

      inline reference operator*() const
      {
        return m_get(m_holding);
      }

      inline bool operator==(const any_raster_const_iterator& other) const
      {
        return m_is_equal(m_holding, other.m_holding);
      }

      inline bool operator!=(const any_raster_const_iterator& other) const
      {
        return !m_is_equal(m_holding, other.m_holding);
      }

      inline T get() const { return m_get(m_holding); }

    private:
      T(*m_get)(const std::any&);
      void(*m_increment)(std::any&);
      bool(*m_is_equal)(const std::any&, const std::any&);

      std::any m_holding;
    };

    template<class T>
    class any_raster
    {
    public:
      any_raster() = default;

      template<class Raster>
      any_raster(Raster r)
        : m_begin(typed_raster_members<Raster>::begin)
        , m_end(typed_raster_members<Raster>::end)
        , m_cbegin(typed_raster_members<Raster>::cbegin)
        , m_cend(typed_raster_members<Raster>::cend)
        , m_rows(typed_raster_members<Raster>::rows)
        , m_cols(typed_raster_members<Raster>::cols)
        , m_size(typed_raster_members<Raster>::size)
        , m_sub_raster(typed_raster_members<Raster>::sub_raster)
        , m_holding(r)
      {}

      using iterator = any_raster_iterator<T>;
      using const_iterator = any_raster_const_iterator<T>;

      iterator begin() { return m_begin(m_holding); }
      iterator end() { return m_end(m_holding); }
      const_iterator begin() const { return m_cbegin(m_holding); }
      const_iterator end() const { return m_cend(m_holding); }
      int rows() const { return m_rows(m_holding); }
      int cols() const { return m_cols(m_holding); }
      int size() const { return m_size(m_holding); }
      any_raster sub_raster(int start_row, int start_col, int rows, int cols) const
      { return m_sub_raster(m_holding, start_row, start_col, rows, cols); }

    private:
      iterator(*m_begin)(std::any&);
      iterator(*m_end)(std::any&);
      const_iterator(*m_cbegin)(const std::any&);
      const_iterator(*m_cend)(const std::any&);
      int(*m_rows)(const std::any&);
      int(*m_cols)(const std::any&);
      int(*m_size)(const std::any&);
      any_raster<T>(*m_sub_raster)(const std::any&, int, int, int, int);

      std::any m_holding;
    };

    template<class Raster>
    any_raster<typename traits<Raster>::value_type>
      make_any_raster(const Raster& r)
    {
      return any_raster<typename traits<Raster>::value_type>(r);
    }
  }
}
