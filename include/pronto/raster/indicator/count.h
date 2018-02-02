
//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================

#pragma once

namespace pronto{
  namespace raster {

    template<class ValueType, class Weight>
    struct count
    {
      count() : m_count(0)
      {}

      void add_sample(const ValueType&, const Weight& w)
      {
        m_count += w;
      }

      void add_sample(const ValueType&)
      {
        ++m_count;
      }

      void subtract_sample(const ValueType&, const Weight& w)
      {
        m_count -= w;
      }

      void subtract_sample(const ValueType&)
      {
        --m_count;
      }

      template<typename Weight>
      void add_subtotal(const count& subtotal, const Weight& w)
      {
        m_count += subtotal.m_count * w;
      }

      void add_subtotal(const count& subtotal)
      {
        m_count += subtotal.m_count;
      }

      template<typename Weight>
      void subtract_subtotal(const count& subtotal, const Weight& w)
      {
        m_count -= subtotal.m_count * w;
      }

      void subtract_subtotal(const count& subtotal)
      {
        m_count -= subtotal.m_count;
      }

      Weight extract() const
      {
        return m_count;
      }

      Weight m_count;
    };

   
    template<class ValueType, class Weight = int>
    struct count_generator
    {
      using indicator = count<ValueType, Weight>;

      indicator operator()() const
      {
        return indicator{};
      }
    };
  }
}
