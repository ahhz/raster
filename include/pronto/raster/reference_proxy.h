//
//=======================================================================
// Copyright 2015-2018
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================

#pragma once

#include <iostream>

namespace pronto {
  namespace raster {

    //Derived must have operator T() and operator=(const T&) member functions
    template<typename Derived, typename T>
    class proxy_reference
    {
      using self_type = Derived;
    public:
      using value_type = T;
      proxy_reference() = default;
      proxy_reference(const proxy_reference&) = default;
      proxy_reference(proxy_reference&&) = default;
      ~proxy_reference() = default;
      operator value_type() const
      {
        return get();
      }
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
        auto temp = get();
        put(--temp);
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
      { auto temp = get(); temp op v; put(temp); return _self(); }
//{ auto temp = _self().operator value_type(); temp op v;_self().operator=(temp); return _self(); }

        // All assigning operators.
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
	
    //the accessor must have put(v) and get() member functions
    template<typename Accessor>
    class put_get_proxy_reference : public proxy_reference<put_get_proxy_reference<Accessor> , typename std::decay<Accessor>::type::value_type >
    {
    public:
      using value_type = typename std::decay<Accessor>::type::value_type;
      put_get_proxy_reference() = delete;
      put_get_proxy_reference(const Accessor& accessor) :m_accessor(accessor)
      {}
      put_get_proxy_reference(const put_get_proxy_reference&) = default;
      put_get_proxy_reference(put_get_proxy_reference&&) = default;

      // conversion to make the iterator readable
      operator value_type() const
      {
        return m_accessor.get();
      }

      // assignment to make the iterator writable
      const put_get_proxy_reference& operator=(const value_type& v) const
      {
        m_accessor.put(v);
        return *this;
      }
      // assignment to make the iterator writable
      const put_get_proxy_reference& operator=(value_type&& v) const
      {
        m_accessor.put(std::move(v));
        return *this;
      }
      
    private:
      Accessor m_accessor;
    };

    template<class CharType, class CharTrait, class Derived, class T>
    auto& operator<<(std::basic_ostream<CharType, CharTrait>& out
        , proxy_reference<Derived, T> const& p)
    {
      out << static_cast<T>(p);
      return out;
    }

    template<class CharType, class CharTrait, class Derived, class T>
    auto& operator >> (std::basic_istream<CharType, CharTrait>& in
        , proxy_reference<Derived, T>& p)
    {
      T v;
      in >> v;
      p = v;
      return in;
    }
  }
}
