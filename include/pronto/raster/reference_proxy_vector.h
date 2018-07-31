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
#include <vector>

namespace pronto {
  namespace raster {
	
    // Acts just like std::vector<Reference>, with as additional behaviour that
    // it can be assigned by std::vector<Value> (provided same size) and can be 
    // cast to std::vector<Value> 
    
    template<class Value, class Reference> 
    class reference_proxy_vector
    {
      using proxied_value_type = Value;
      using vector_type = std::vector<Reference>;
      using proxied_vector_type = std::vector<Value>;

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


      reference_proxy_vector& operator=(const proxied_vector_type& that)
      {
        assert(that.size() == m_vector.size());

        auto a = m_vector.begin();
        auto b = that.begin();
        auto b_end = that.end();
        for (; b != b_end; ++a, ++b) {
          *a = *b;
        }
        return *this;
      }

      reference_proxy_vector& operator=(proxied_vector_type&& that)
      {
        m_vector.resize(that.size());
        auto a = m_vector.begin();
        auto b = that.begin();
        auto b_end = that.end();
        for (; b != b_end; ++a, ++b) {
          *a = std::move(*b);
        }
        that.clear();
        return *this;
      }

    private:
      vector_type m_vector;
    
    public:

// MACRO for forwarding member functions of m_vector
#define PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(func) \
template<class... Args> auto func(Args&&... args) ->       \
decltype(m_vector.func(std::forward<Args>(args)...))       \
{ return m_vector.func(std::forward<Args>(args)...); }     

      // Forwarding all member functions of m_vector
      // For some compilers (GCC) this must happen after the declaration of 
      // m_vector
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(assign)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(get_allocator)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(at)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(front)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(back)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(data)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(begin)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(cbegin)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(end)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(cend)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(rbegin)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(crbegin)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(rend)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(crend)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(empty)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(size)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(max_size)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(reserve)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(capacity)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(shrink_to_fit)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(clear)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(insert)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(emplace)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(erase)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(push_back)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(emplace_back)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(pop_back)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(resize)
      PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(swap)
     
      // Visual Studio did not like [] inside macro so, expand 
      // PRONTO_RASTER_REFERENCE_PROXY_VECTOR_FUNCTION(operator[])

      template<class... Args> auto operator[](Args&&... args) ->      
        decltype(m_vector[std::forward<Args>(args)...])      
      { return m_vector[std::forward<Args>(args)...]; }
     
    };
  }
}
