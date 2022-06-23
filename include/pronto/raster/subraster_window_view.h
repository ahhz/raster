//
//=======================================================================
// Copyright 2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
#pragma once

#include <pronto/raster/padded_raster_view.h>
#include <pronto/raster/traits.h>

#include <iterator>
#include <utility>

namespace pronto {
  namespace raster {
    template<class Raster>
    class subraster_window_view; // forward declaration

    template<class Raster, bool IsMutable> 
    class subraster_window_iterator
    { 
      using view_type = typename std::conditional<IsMutable
        , subraster_window_view<Raster>
        , const subraster_window_view<Raster>>::type;
      using padded_raster_type = typename std::conditional<IsMutable
        , padded_raster_view<Raster>, const padded_raster_view<Raster> >::type;
    public:
      using value_type = typename traits<padded_raster_type>::sub_raster;
      using reference = value_type;
      using pointer = void;
      using difference_type = std::ptrdiff_t;
      using iterator_category = std::input_iterator_tag;

      subraster_window_iterator& operator++()
      {
        increment();
         return *this;
      }

      subraster_window_iterator& operator+=(const difference_type& n)
      {
        increment_step(n);
        return *this;
      }
      
      subraster_window_iterator operator++(int)
      {
        subraster_window_iterator temp(*this);
        ++(*this);
        return temp;
      }
     
      subraster_window_iterator operator+(const difference_type& n) const
      {
        subraster_window_iterator temp(*this);
        temp += n;
        return temp;
      }

      reference operator[](std::ptrdiff_t distance) const
      {
        return *(operator+(distance));
      }

      subraster_window_iterator& operator--()
      {
        decrement();
        return *this;
      }

      subraster_window_iterator operator--(int)
      {
        subraster_window_iterator temp(*this);
        --(*this);
        return temp;
      }

      subraster_window_iterator operator-(const difference_type& n) const
      {
        subraster_window_iterator temp(*this);
        temp -= n;
        return temp;
      }

      subraster_window_iterator& operator-=(const difference_type& n)
      {
        decrement_step(n);
        return *this;
      }


      reference operator*() const
      {
        return dereference();
      }

      bool operator==(const subraster_window_iterator& b) const
      {
        return m_index == b.m_index;
      }

      bool operator!=(const subraster_window_iterator& b) const
      {
        return m_index != b.m_index;
      }

      bool operator<(const subraster_window_iterator& b) const
      {
        return m_index < b.m_index;
      }

      bool operator>(const subraster_window_iterator& b) const
      {
        return m_index > b.m_index;
      }

      bool operator<=(const subraster_window_iterator& b) const
      {
        return m_index <= b.m_index;
      }

      bool operator>=(const subraster_window_iterator& b) const
      {
        return m_index>= b.m_index;
      }

    private:
      void increment()
      {
        ++m_index;
      }
      void decrement()
      {
        --m_index;
      }

      void increment_step(const difference_type& n)
      {
        m_index+=n;
      }
      void decrement_step(const difference_type& n)
      {
        m_index -= n;
      }
      
      reference dereference() const
      {
        return m_view->get_raster().sub_raster(
          static_cast<int>(m_index / m_view->cols()), 
          static_cast<int>(m_index % m_view->cols()),
          m_view->window_rows(), m_view->window_cols());
      }
    private: 
      friend class subraster_window_view<Raster>;
      view_type* m_view;
      std::ptrdiff_t m_index;
    };

    template<class Raster>
    class subraster_window_view
    {
    using value_type = typename std::ranges::range_value_t<Raster>;
    public:
      subraster_window_view(const Raster& raster, int window_rows_before, int window_rows_after
        , int window_cols_before, int window_cols_after, value_type pad_value)
        : m_window_rows_before(window_rows_before), m_window_rows_after( window_rows_after)
        , m_window_cols_before(window_cols_before), m_window_cols_after(window_cols_after)
        , m_padded_raster(pad(raster, window_rows_before,window_rows_after
          , window_cols_before, window_cols_after, pad_value))
        , m_rows(raster.rows()), m_cols(raster.cols()), m_first_row(0), m_first_col(0)
      {}
      subraster_window_view() = default;
      subraster_window_view(const subraster_window_view&) = default;
      subraster_window_view(subraster_window_view&&) = default;
      subraster_window_view& operator=(const subraster_window_view&) = default;
      subraster_window_view& operator=(subraster_window_view&&) = default;
      
      using iterator = subraster_window_iterator<Raster, true>;
      using const_iterator = subraster_window_iterator<Raster, false>;

      int rows() const
      {
        return m_rows;
      }
      
      int cols() const
      {
        return m_cols;
      }
      
      int size() const
      {
        return rows() * cols();
      }
      
      subraster_window_view sub_raster(int first_row, int first_col, int rows, int cols) const
      {
        subraster_window_view sub(*this);
        sub.m_first_row = m_first_row + first_row;
        sub.m_first_col = m_first_col + first_col;
        sub.m_rows = rows;
        sub.m_cols = cols;
        sub.m_padded_raster = m_padded_raster.sub_raster(
          sub.m_first_row, sub.m_first_col
          , rows + m_window_rows_before + m_window_rows_after
          , cols + m_window_cols_before + m_window_cols_after);
        return sub;
      }

      iterator begin()
      {
        iterator i;
        i.m_view = this;
        i.m_index = 0;
        return i;
      }

      iterator end()
      {
        iterator i;
        i.m_view = this;
        i.m_index = size();
        return i;
      }

      const_iterator begin() const
      {
        const_iterator i;
        i.m_view = this;
        i.m_index = 0;
        return i;

      }
      const_iterator end() const
      {
        const_iterator i;
        i.m_view = this;
        i.m_index = size();
        return i;
      }

    private:
      friend class iterator;
      friend class const_iterator;
      int window_rows() const
      {
        return m_window_rows_before + m_window_rows_after + 1;
      }

      int window_cols() const
      {
        return m_window_cols_before + m_window_cols_after + 1;
      }

      padded_raster_view<Raster>& get_raster()
      {
        return m_padded_raster;
      }

      const padded_raster_view<Raster>& get_raster() const
      {
        return m_padded_raster;
      }

    private:
      int m_window_rows_before;
      int m_window_rows_after;
      int m_window_cols_before;
      int m_window_cols_after;
      int m_first_row;
      int m_first_col;
      int m_rows;
      int m_cols;
      padded_raster_view<Raster> m_padded_raster;
    };

    template<class Raster, class ValueType>
    subraster_window_view<Raster> make_subraster_window_view(const Raster& raster,
      int rows_before, int rows_after, int cols_before, int cols_after, const ValueType& pad_value)
    {
      return subraster_window_view<Raster>(raster, rows_before, rows_after, cols_before, cols_after, pad_value);
    }

    template<class Raster, class ValueType>
    subraster_window_view<Raster> make_square_subraster_window_view(const Raster& raster,
      int radius, const ValueType& pad_value)
    {
      return subraster_window_view<Raster>(raster, radius, radius, radius, radius, pad_value);
    }
  }
}
