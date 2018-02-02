//
//=======================================================================
// Copyright 2015-2107
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once

#include <pronto/raster/traits.h>

#include <iterator>

namespace pronto {
  namespace raster {

    template<class T> //required to be a Raster and a View
    class uniform_raster_view
    {
    public:
      class iterator
      {
      public:
        using reference = const T&;
        using value_type = T;
        using pointer = void;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        iterator()
        {}
        
        iterator& operator++()
        { 
          ++m_index;
          return *this;
        }

        iterator operator++(int) 
        {
          iterator copy = (*this);
          ++(*this);
          return copy;
        }

        iterator& operator+=(const difference_type& n)
        {
          m_index += n;
          return *this;
        }

        iterator operator+(const difference_type& n) const
        {
          iterator temp(*this);
          temp += n;
          return temp;
        }

        iterator& operator--() 
        {
          --m_index;
          return *this;
        }

        iterator operator--(int)
        {
          iterator copy = (*this);
          --(*this);
          return copy;
        }

        iterator& operator-=(const difference_type& n)
        {
          m_index -= n;
          return *this;
        }

        iterator operator-(const difference_type& n) const
        {
          iterator temp(*this);
          temp -= n;
          return temp;
        }

        bool operator==(const iterator& that) const
        {
          return that.m_index == m_index;
        }

        bool operator!=(const iterator& that) const
        {
          return that.m_index != m_index;
        }

        const T& operator*() const
        {
          return m_view->m_value;
        }

        const T& operator[](std::ptrdiff_t) const
        {
          return m_view->m_value;
        }

        bool operator<(const iterator& that) const
        {
          return that.m_index < m_index;
        }

        bool operator<=(const iterator& that) const
        {
          return that.m_index <= m_index;
        }

        bool operator>(const iterator& that) const
        {
          return that.m_index > m_index;
        }

        bool operator>=(const iterator& that) const
        {
          return that.m_index >= m_index;
        }

      private:
        friend class uniform_raster_view;
        void find_begin(const uniform_raster_view* view)
        {
          m_view = view;
          m_index = 0;
        }
        void find_end(const uniform_raster_view* view)
        {
          m_view = view;
          m_index = view->size();
        }

        int m_index;
        const uniform_raster_view* m_view;
      };

      uniform_raster_view() : m_rows(0), m_cols(0), m_value(T{})
      {}

      uniform_raster_view(int rows, int cols, const T& value)
        : m_rows(rows), m_cols(cols), m_value(value)
      {}

      int rows() const { return m_rows; }
      int cols() const { return m_cols; }
      int size() const { return m_rows * m_cols; }
    
      iterator begin() const 
      {
        iterator i;
        i.find_begin(this);
        return i;
      }

      iterator end() const
      {
        iterator i;
        i.find_end(this);
        return i;
      }

      uniform_raster_view sub_raster(int, int, int rows, int cols) const
      {
        return uniform_raster_view(rows, cols, m_value);
      }
    private:
      int m_rows;
      int m_cols;
      T m_value;
    };

    template<class T>
    uniform_raster_view<T> uniform(int rows, int cols, T value)
    {
      return uniform_raster_view<T>(rows, cols, value);
    }
  }
}
