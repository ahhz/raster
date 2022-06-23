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
#include <pronto/raster/iterator_facade.h>
#include <pronto/raster/traits.h>
#include <ranges>


namespace pronto {
  namespace raster {

    template<class T> //required to be a Raster and a View
    class uniform_raster_view : public std::ranges::view_interface < uniform_raster_view<T>>
    {
    public:
      class iterator: public iterator_facade<iterator>
      {
      public:
        static const bool is_mutable = false;
        static const bool is_single_pass = false;

        iterator()
        {}
        
        void increment()
        { 
          ++m_index;
        }

        void advance(const std::ptrdiff_t & n)
        {
          m_index += n;
        }

        void decrement() 
        {
          --m_index;
        }


        bool equal_to(const iterator& that) const
        {
          return that.m_index == m_index;
        }

        std::ptrdiff_t distance_to(const iterator& that) const
        {
          return that.m_index - m_index;
        }


        const T& dereference() const
        {
          return m_view->m_value;
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
      return uniform_raster_view(rows, cols, value);
    }
  }
}
