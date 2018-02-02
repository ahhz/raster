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
#include <pronto/raster/reference_proxy.h>
#include <pronto/raster/traits.h>

#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>

namespace pronto {
  namespace raster {
    //// Forward declaration for friendship;
     template<class Raster> class padded_raster_view;

    template<class Raster, class Iterator>
    class padded_raster_iterator
     {
    public:
      using value_type = typename std::iterator_traits<Iterator>::value_type;
      using reference = reference_proxy<padded_raster_iterator>;
      using difference_type = void;
      using pointer = void;
      using iterator_category = std::input_iterator_tag;
      
      padded_raster_iterator() = default;

      padded_raster_iterator(const padded_raster_view<Raster>* view,
        Iterator iter) : m_view(view), m_iter(iter)
      {
        find_begin(view, iter);
      }

      padded_raster_iterator& operator++()
      {
        ++m_index_in_stretch;
        if (!m_is_padding) ++m_iter;
        
        while (m_index_in_stretch == m_stretch_size) {
          ++m_stretch;
          if (m_stretch == m_view->num_stretches()) {
            m_index_in_stretch = 0;
            return *this;
          }
                
          m_index_in_stretch = 0;
          m_stretch_size = m_view->length_of_stretch(m_stretch);
          m_is_padding = m_view->is_padding(m_stretch);
        }
        return *this;
      }

      padded_raster_iterator operator++(int)
      {
        padded_raster_iterator temp(*this);
        ++(*this);
        return temp;
      }

      reference operator*() const
      {
        return reference(*this);
      }

      value_type get() const
      {
        if (m_is_padding) {
          return m_view->m_pad_value;
        }
        else {
          return *m_iter;
        }
      }

      void put(const value_type& value) const
      {
        if (m_is_padding)
        {
          throw("trying to write into the padding of a padded_raster_view");
        }
        *m_iter = value;
      }

    public:
      friend bool operator==(const padded_raster_iterator& a,
        const padded_raster_iterator& b)
      {
        return a.m_stretch == b.m_stretch && a.m_index_in_stretch == b.m_index_in_stretch;
      }

      friend bool operator!=(const padded_raster_iterator& a,
        const padded_raster_iterator& b)
      {
        return a.m_stretch != b.m_stretch || a.m_index_in_stretch != b.m_index_in_stretch;
      }

    private:

      template<typename U>
      friend class padded_raster_view;

      void find_begin(const padded_raster_view<Raster>* view, Iterator iter)
      {
        m_view = view;
        m_is_padding = true;
        m_stretch = 0;
        m_index_in_stretch = 0;
        m_stretch_size = m_view->length_of_stretch(0);
        m_iter = iter;

        while (m_stretch_size == 0 ) {
          ++m_stretch;
          if (m_stretch == m_view->num_stretches()) {
            return;
          }
          m_stretch_size = m_view->length_of_stretch(m_stretch);
          m_is_padding = m_view->is_padding(m_stretch);
        }
      }

      void find_end(const padded_raster_view<Raster>* view, Iterator iter)
      {
        m_view = view;
        m_iter = iter;
        m_stretch = m_view->num_stretches();
        m_index_in_stretch = 0;
        m_is_padding = false;
      }
   private:
      int m_stretch;
      int m_stretch_size;
      int m_index_in_stretch;

      bool m_is_padding;

      Iterator m_iter;

      const padded_raster_view<Raster>* m_view;
    };
   
    template<typename Raster> // requires Raster and View concepts 
    class padded_raster_view
    {
    private:
      using value_type = typename traits<Raster>::value_type;

    public:
      using iterator 
        = padded_raster_iterator<Raster, typename traits<Raster>::iterator>;
      using const_iterator
        = padded_raster_iterator<Raster, typename traits<Raster>::const_iterator>;
      
      padded_raster_view() = default;

      padded_raster_view(const Raster& raster,
        int leading_rows, int trailing_rows, 
        int leading_cols, int trailing_cols, const value_type& value) 
        : m_raster(raster), m_pad_value(value)
        , m_leading_rows(leading_rows), m_trailing_rows(trailing_rows)
        , m_leading_cols(leading_cols), m_trailing_cols(trailing_cols)
        , m_core_rows(raster.rows()), m_core_cols(raster.cols())
        , m_rows(raster.rows() + leading_rows + trailing_rows)
        , m_cols(raster.cols() + leading_cols + trailing_cols)
      {}
           
      iterator begin() 
      {
        iterator i(this, m_raster.begin());
        i.find_begin(this, m_raster.begin());
        return i;
      }

      iterator end() 
      {
        iterator i(this, m_raster.begin());
        i.find_end(this, m_raster.end());
        return i;
      }

      const_iterator begin() const
      {
        const_iterator i(this, m_raster.begin());
        i.find_begin(this, m_raster.begin());
        return i;
      }

      const_iterator end() const
      {
        const_iterator i(this, m_raster.begin());
        i.find_end(this, m_raster.end());
        return i;
      }

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

      padded_raster_view<typename traits<Raster>::sub_raster>
        sub_raster(int start_row, int start_col, int rows, int cols) const
      {
        assert(start_row >= 0 
          && start_col >= 0 
          && start_row + rows <= m_rows 
          && start_col + cols <= m_cols
          && rows >= 0
        && cols >= 0);

        int leading_rows = std::max(0, m_leading_rows - start_row);
        int leading_cols = std::max(0, m_leading_cols - start_col);

        int trailing_rows = std::max(0, m_trailing_rows 
          - (m_rows - start_row - rows) );
        int trailing_cols = std::max(0, m_trailing_cols 
          - (m_cols - start_col - cols) );

        int core_rows = std::max(0, rows - leading_rows - trailing_rows);
        int core_cols = std::max(0, cols - leading_cols - trailing_cols);

        int first_core_row = std::max(0, start_row - m_leading_rows);
        int first_core_col = std::max(0, start_col - m_leading_cols);

        return padded_raster_view<typename traits<Raster>::sub_raster>(
          m_raster.sub_raster
            (first_core_row, first_core_col, core_rows, core_cols)
          , leading_rows, trailing_rows, leading_cols, trailing_cols
          , m_pad_value);
      }

    public:
      friend class padded_raster_iterator<Raster, iterator> ; 
      friend class padded_raster_iterator<Raster, const_iterator>; 
      
      int num_stretches() const 
      {
        return m_core_rows * 3 + 2;
      }

      int length_of_stretch(int stretch) const
      {
        switch (stretch % 3)
        {
        case 0:
          if (stretch == 0) return m_leading_rows * m_cols;
          else return m_trailing_cols;
        case 1:
          if (stretch == (num_stretches() - 1)) return m_trailing_rows * m_cols;
          else return m_leading_cols;
        case 2:
        default: // suppress warning
          return m_core_cols;
        }
      }

      int get_index(int stretch, int index_in_stretch) const
      {
        switch (stretch % 3)
        {
        case 0:
          if (stretch == 0) 
          {
            return index_in_stretch;
          }
          else 
          {
            return ( (stretch - 1) / 3 + m_leading_rows) * m_cols 
              + m_leading_cols + m_core_cols + index_in_stretch;
          }
        case 1:
          if (stretch == num_stretches() - 1) 
          {
            return (m_leading_rows + m_core_rows) * m_cols + index_in_stretch;
          }
          else 
          {
            return ((stretch - 1) / 3 + m_leading_rows) * m_cols 
              + index_in_stretch;
          }
        case 2:
        default:
          return ((stretch - 1) / 3 + m_leading_rows) * m_cols 
            + m_leading_cols +index_in_stretch;
        }
      }
      
      std::pair<int, int> get_stretch_and_index_in_stretch(int index) const
      {
        int stretch, index_in_stretch;
        int row = index / m_cols;
        int col = index % m_cols;

        if (row < m_leading_rows) {
          stretch = 0;
          index_in_stretch = index;
        } else if (row < m_leading_rows + m_core_rows) {
          if (col < m_leading_cols) {
            stretch = 1 + (col - m_leading_cols) * 3;
            index_in_stretch = col;

          } else if (col < m_leading_cols + m_core_cols)  {
            stretch = 2 + (col - m_leading_cols) * 3;
            index_in_stretch = col - m_leading_cols;
          }  else {
            stretch = 3 + (col - m_leading_cols) * 3;
            index_in_stretch = col - m_leading_cols - m_core_cols;
          }
        }  else {
          stretch = num_stretches() - 1;
          index_in_stretch = index - (m_leading_rows + m_core_rows) * m_cols;
        }
        return std::make_pair(stretch, index_in_stretch);
      }
     
      bool is_padding(int stretch) const
      {
        return  (stretch % 3) != 2;
      }

      Raster m_raster;

      int m_leading_rows;
      int m_leading_cols;
      int m_trailing_rows;
      int m_trailing_cols;
      int m_core_rows;
      int m_core_cols;
      int m_rows;
      int m_cols;
      
      value_type m_pad_value;
    };

    template<class Raster>
      padded_raster_view<Raster> pad(Raster raster,
        int rows_before, int rows_after, int cols_before, int cols_after
        , const typename traits<Raster>::value_type& value) {

      return padded_raster_view<Raster>(raster, rows_before, rows_after
        , cols_before, cols_after, value);
    }
  }
}
