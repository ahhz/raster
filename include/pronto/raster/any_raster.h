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
#include <pronto/raster/reference_proxy.h>
#include <pronto/raster/traits.h>

#include <any>
#include <cassert>
#include <functional>
#include <type_traits>

namespace pronto {
  namespace raster {

    //Derived must have operator T() and operator=(const T&) member functions
    template<typename Derived, typename T>
    class proxy_reference
    {
      using self_type = Derived;
      using value_type = T;

    public:
      proxy_reference() = default;
      proxy_reference(const proxy_reference&) = default;
      proxy_reference(proxy_reference&&) = default;
      
      void put(const value_type& v) const {
        _self() = v;
      }

      value_type get() const {
        return static_cast<value_type>(_self());
      }

      const self_type& operator++() const
      {
        auto temp = get();
        put(++temp);
        return _self();
      }
      const self_type& operator--() const
      {
        put(--get());
        return _self();
      }

      value_type operator++(int) const
      {
        value_type temp = get();
        put(++get());
        return temp;
      }
      value_type operator--(int) const
      {
        value_type temp = get();
        put(--get());
        return temp;
      }

    private:
      self_type& _self() {
        return static_cast<self_type&>(*this);
      }

      const self_type& _self() const {
        return static_cast<const self_type&>(*this);
      }
    public:

      // MACRO to implement assigning operator
#define PRONTO_RASTER_PROXY_REFERENCE_ASSIGNING_OPERATOR(op)      \
template<class U> const self_type& operator op(const U& v) const \
{ auto temp = _self().operator value_type(); temp op v;_self().operator=(temp); return _self(); }

      // All assigning operators.
      // For the sake of some compilers deeclared after m_accessors
      PRONTO_RASTER_PROXY_REFERENCE_ASSIGNING_OPERATOR(+= )
        PRONTO_RASTER_PROXY_REFERENCE_ASSIGNING_OPERATOR(-= )
        PRONTO_RASTER_PROXY_REFERENCE_ASSIGNING_OPERATOR(/= )
        PRONTO_RASTER_PROXY_REFERENCE_ASSIGNING_OPERATOR(*= )
        PRONTO_RASTER_PROXY_REFERENCE_ASSIGNING_OPERATOR(%= )
        PRONTO_RASTER_PROXY_REFERENCE_ASSIGNING_OPERATOR(&= )
        PRONTO_RASTER_PROXY_REFERENCE_ASSIGNING_OPERATOR(|= )
        PRONTO_RASTER_PROXY_REFERENCE_ASSIGNING_OPERATOR(^= )
        PRONTO_RASTER_PROXY_REFERENCE_ASSIGNING_OPERATOR(<<= )
        PRONTO_RASTER_PROXY_REFERENCE_ASSIGNING_OPERATOR(>>= )
    };


    template<class T, access AccessType = access::read_write>
    class type_erased_reference : public proxy_reference<type_erased_reference<T,AccessType>, T>
    {
      static const bool is_mutable = AccessType != access::read_only;

    public:
      type_erased_reference(const type_erased_reference&) = default;
      type_erased_reference(type_erased_reference&&) = default;
      ~type_erased_reference() = default;
      type_erased_reference& operator=(const type_erased_reference& other)
      {
        m_put(other.m_get());
        return *this;
      }
      type_erased_reference& operator=(type_erased_reference&& other)
      {
        m_put(other.m_get());
        return *this;

      }
      type_erased_reference(T& r)
       {
         auto ref = std::ref(r);
         m_get = [ref]() {return ref.get(); };
         
         if constexpr (is_mutable) {
           m_put = [ref](const T& v) {ref.get() = v; };
         }
         else {
           m_put = [](const T&) {assert(false); };
         }
       }

       type_erased_reference(const T& r)
       {
         auto ref = std::ref(r);
         m_get = [ref]() {return ref.get(); };
         m_put = [](const T&) {assert(false); };
       }

       template<class Proxy>
       type_erased_reference(const Proxy& p)
       {
         m_get = [p]() {return static_cast<T>(p); };
         if constexpr (is_mutable) {
           m_put = [p](const T& v) {p = v; };
         }
         else {
           m_put = [](const T&) {assert(false); };
         }
       }
        operator T() const 
        {
          return m_get();
        }

       const type_erased_reference& operator=(const T& value) const
         requires is_mutable
       {
         m_put(value);
         return *this;
       }

       std::function<T()> m_get;
       std::function<void(T)> m_put;
    };

    template<class T, class IterationType = multi_pass, access AccessType = access::read_write>
    class type_erased_raster_iterator : public iterator_facade<type_erased_raster_iterator<T> >
    {

    public:
      static const bool is_mutable = AccessType != access::read_only;
      static const bool is_single_pass = std::is_same_v<IterationType, single_pass>;
      type_erased_raster_iterator() = default;
      type_erased_raster_iterator(const  type_erased_raster_iterator& iter) = default;
      type_erased_raster_iterator(type_erased_raster_iterator&& iter) = default;
      type_erased_raster_iterator& operator=(const type_erased_raster_iterator&) = default;
      type_erased_raster_iterator& operator=(type_erased_raster_iterator&&) = default;
      ~type_erased_raster_iterator() = default;

      template<class Iter>
       type_erased_raster_iterator(const Iter& iter)
      {
        using iter_type = std::remove_cv_t<Iter>;
        m_any_iter = iter;
        m_increment = [&]() {++std::any_cast<iter_type&>(m_any_iter); };
        m_decrement = [&]() {--std::any_cast<iter_type&>(m_any_iter); };
        m_advance = [&](std::ptrdiff_t offset) {std::any_cast<iter_type&>(m_any_iter) += offset; } ;
        m_equal_to = [&](const type_erased_raster_iterator& other)
        {
          return std::any_cast<iter_type&>(m_any_iter) == std::any_cast<const iter_type&>(other.m_any_iter); 
        };
        
        m_distance_to = [&](const type_erased_raster_iterator& other)
        { 
          return std::any_cast<const iter_type&>(other.m_any_iter) - std::any_cast<iter_type&>(m_any_iter); 
        };
        
        m_dereference = [&]() {return type_erased_reference<T>(*(std::any_cast<iter_type&>(m_any_iter))); };

      }

       type_erased_reference<T, AccessType> dereference() const { return m_dereference(); };
       void increment() { m_increment(); }
       void decrement() { m_decrement(); }
       void advance(std::ptrdiff_t offset) { m_advance(offset); }
       bool equal_to(const type_erased_raster_iterator& other) const { return m_equal_to(other); };
       std::ptrdiff_t distance_to(const type_erased_raster_iterator& other) const { return m_distance_to(other); };

    private:
      std::function<type_erased_reference<T,AccessType>()> m_dereference;
      std::function<void()> m_increment;
      std::function<void()> m_decrement;
      std::function<void(std::ptrdiff_t )> m_advance;
      std::function<bool(const type_erased_raster_iterator& )> m_equal_to;
      std::function<std::ptrdiff_t(const type_erased_raster_iterator& )> m_distance_to;
     
      std::any m_any_iter;
    };

    template<class T, class IterationType = multi_pass, access AccessType = access::read_write>
    class type_erased_raster : public std::ranges::view_interface<type_erased_raster<T, IterationType, AccessType> >
    {
    public:
      static const bool is_mutable = AccessType != access::read_only;
      static const bool is_single_pass = std::is_same_v<IterationType, single_pass>;

      using iterator = typename type_erased_raster_iterator<T, IterationType, AccessType>;

      type_erased_raster() {};
      ~type_erased_raster() {};
      template<class Raster>
      type_erased_raster(const Raster& r) {
        m_raster = r;
        using raster_type= std::remove_cv_t<Raster>;
        m_begin = [&]() {return iterator( std::any_cast<raster_type&>(m_raster).begin() ); };
        m_end =   [&]() {return iterator(std::any_cast<raster_type&>(m_raster).end()); };
        m_rows =  [&]() {return std::any_cast<raster_type&>(m_raster).rows(); };
        m_cols =  [&]() {return std::any_cast<raster_type&>(m_raster).cols(); };
        m_size =  [&]() {return std::any_cast<raster_type&>(m_raster).size(); };
        m_sub_raster = [&](int a, int b, int c, int d)
        {
          return type_erased_raster<T>(std::any_cast<raster_type&>(m_raster).sub_raster(a,b,c,d) );
        };
      };

      iterator begin() const { return m_begin(); }
      iterator end()   const { return m_end(); }
      int rows()       const { return m_rows(); }
      int cols()       const { return m_cols(); }
      int size()       const { return m_size(); }
      type_erased_raster sub_raster(int a, int b, int c, int d) const { return m_sub_raster(a, b, c, d); }
   
    private:
      std::function<iterator()> m_begin;
      std::function<iterator()> m_end;
      std::function<int()> m_rows;
      std::function<int()> m_cols;
      std::function<int()> m_size;
      std::function <type_erased_raster(int, int, int, int)> m_sub_raster;

      std::any m_raster;

    };

   // template<class T, class IterationType = multi_pass, access AccessType = access::read_write>
   // using any_raster = type_erased_raster<T, IterationType, AccessType>;

    
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
