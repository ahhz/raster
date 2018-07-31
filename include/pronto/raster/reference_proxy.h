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
	
    //the accessor must have put(v) and get() member functions
    template<typename Accessor>
    class reference_proxy
    {
    public:
      using value_type = typename std::decay<Accessor>::type::value_type;
      reference_proxy() = delete;
      reference_proxy(const Accessor& accessor) :m_accessor(accessor)
      {}
      reference_proxy(const reference_proxy&) = default;
      reference_proxy(reference_proxy&&) = default;

      // conversion to make the iterator readable
      operator value_type() const
      {
        return m_accessor.get();
      }

      // assignment to make the iterator writable
      reference_proxy& operator=(const value_type& v)
      {
        m_accessor.put(v);
        return *this;
      }

      template<class OtherAccessor>
      reference_proxy& operator=(const reference_proxy<OtherAccessor>& that)
      {
        using other_value_type = typename reference_proxy<OtherAccessor>::value_type;
        return operator=(static_cast<other_value_type>(that));
      }

      reference_proxy& operator=(const reference_proxy& that)
      {
        return operator=(static_cast<value_type>(that));
      }
      
      reference_proxy& operator=(reference_proxy&& that)
      {
        return operator=(static_cast<value_type>(that));
      }

      const reference_proxy& operator++() 
      {
        m_accessor.put(++m_accessor.get());
        return *this;
      }
      const reference_proxy& operator--() 
      {
        m_accessor.put(--m_accessor.get());
        return *this;
      }

      value_type operator++(int)
      {
        value_type temp = m_accessor.get();
        m_accessor.put(++m_accessor.get());
        return temp;
      }
      value_type operator--(int)
      {
        value_type temp = m_accessor.get();
        m_accessor.put(--m_accessor.get());
        return temp;
      }
    private:
      Accessor m_accessor;
    public:

#define PRONTO_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(op)      \
template<class T> const reference_proxy& operator op(const T& v) \
{ auto temp = m_accessor.get(); temp op v;m_accessor.put(temp); return *this; }
      
      PRONTO_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(+=)
      PRONTO_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(-= )
      PRONTO_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(/= )
      PRONTO_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(*= )
      PRONTO_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(%= )
      PRONTO_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(&= )
      PRONTO_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(|= )
      PRONTO_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(^= )
      PRONTO_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(<<= )
      PRONTO_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(>>= )
        
    };

    template<class CharType, class CharTrait, class Accessor>
    std::basic_ostream<CharType, CharTrait>&
      operator<<(std::basic_ostream<CharType, CharTrait>& out
        , reference_proxy<Accessor> const& p)
    {
      using value_type = typename reference_proxy<Accessor>::value_type;
      out << static_cast<value_type>(p);
      return out;
    }

    template<class CharType, class CharTrait, class Accessor>
    std::basic_istream<CharType, CharTrait>&
      operator >> (std::basic_istream<CharType, CharTrait>& in
        , reference_proxy<Accessor>& p)
    {
      using value_type = typename reference_proxy<Accessor>::value_type;
      value_type v;
      in >> v;
      p = v;
      return in;
    }
  }
}
