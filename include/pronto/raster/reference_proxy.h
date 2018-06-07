//
//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================

#pragma once

#include <iostream>
#include <vector>

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
#define BLINK_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(op)      \
template<class T> const reference_proxy& operator op(const T& v) \
{ auto temp = m_accessor.get(); temp op v;m_accessor.put(temp); return *this; }
      
      BLINK_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(+=)
      BLINK_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(-= )
      BLINK_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(/= )
      BLINK_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(*= )
      BLINK_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(%= )
      BLINK_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(&= )
      BLINK_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(|= )
      BLINK_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(^= )
      BLINK_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(<<= )
      BLINK_RASTER_REFERENCE_PROXY_ASSIGNING_OPERATOR(>>= )
        
    private:
      Accessor m_accessor;
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

    template<class Value, class Reference> 
    class reference_proxy_vector
    {
      using proxied_value_type = Value;
      using vector_type = std::vector<Reference>;
      using proxied_vector_type = std::vector<Reference>;

    public:
      using allocator_type = typename vector_type::allocator_type;
      using value_type = typename vector_type::value_type;
      using iterator = typename vector_type::iterator;
      using const_iterator = typename vector_type::const_iterator;
      using size_type = typename vector_type::size_type;
      using reference = typename vector_type::reference;
      using size_type = typename vector_type::size_type;
      using difference_type = typename vector_type::difference_type;
      using const_reference = typename vector_type::const_reference;
      using const_pointer = typename vector_type::const_pointer;
      using pointer = typename vector_type::pointer;
      using reverse_iterator = typename vector_type::reverse_iterator;
      using const_reverse_iterator = typename vector_type::const_reverse_iterator;

      operator proxied_vector_type() const
      {
        proxied_vector_type out;
        out.reserve(m_vector.size());
        for (auto&& i : m_vector) {
          out.push_back(i);
        }
        return out;
      }

      reference_proxy_vector() = default;

      template<class... Args>
      reference_proxy_vector(Args&&... args) : m_vector(std::forward<Args>(args)...)
      {
 //       m_vector.operator=(std::forward<Args>(args)...);
  //      return *this;
      }

      reference_proxy_vector(reference_proxy_vector&& that)
      {
        m_vector = std::move(that.m_vector);
      }
      reference_proxy_vector(const reference_proxy_vector& that)
      {
        m_vector = that.m_vector;
      }

      reference_proxy_vector(const std::vector<value_type>& that)
      {
        m_vector.resize(that.size());
        auto a = m_vector.begin();
        auto b = that.begin();
        auto b_end = that.end();
        for (; b != b_end; ++a, ++b){
          *a = *b;
        }
        return *this;
      }


      reference_proxy_vector(std::vector<value_type>&& that)
      {
        m_vector.resize(that.size());
        auto a = m_vector.begin();
        auto b = that.begin();
        auto b_end = that.end();
        for (; b != b_end; ++a, ++b) {
          *a = *b;
        }
        return *this;
      }
      
      reference_proxy_vector& operator=(const std::vector<value_type>& that)
      {
        m_vector.resize(that.size());
        auto a = m_vector.begin();
        auto b = that.begin();
        auto b_end = that.end();
        for (; b != b_end; ++a, ++b) {
          *a = *b;
        }
        return *this;
      }

      reference_proxy_vector& operator=(std::vector<value_type>&& that)
      {
        m_vector.resize(that.size());
        auto a = m_vector.begin();
        auto b = that.begin();
        auto b_end = that.end();
        for (; b != b_end; ++a, ++b) {
          *a = *b;
        }
        return *this;
      }


#define BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(func) \
template<class... Args> auto func(Args&&... args) ->       \
decltype(m_vector.func(std::forward<Args>(args)...))       \
{ return m_vector.func(std::forward<Args>(args)...); }     


      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(assign)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(get_allocator)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(at)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(front)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(back)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(data)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(begin)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(cbegin)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(end)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(cend)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(rbegin)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(crbegin)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(rend)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(crend)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(empty)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(size)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(max_size)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(reserve)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(capacity)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(shrink_to_fit)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(clear)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(insert)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(emplace)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(erase)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(push_back)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(emplace_back)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(pop_back)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(resize)
      BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(swap)
     
      // Visual Studio did not like [] inside macro so, expand 
      // BLINK_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(operator[])

      template<class... Args> auto operator[](Args&&... args) ->      
        decltype(m_vector[std::forward<Args>(args)...])      
      { return m_vector[std::forward<Args>(args)...]; }
     

    private:
      vector_type m_vector;
    };
  }
}
