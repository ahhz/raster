//
//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================       
//
// The rectangle_window_iterator provides a generalized interface for patch
// or cell-based indicators
//

#pragma once

#include <blink/raster/indicator_functions.h>
#include <blink/raster/optional.h>
#include <blink/raster/traits.h>

#include <iterator>
#include <vector>

namespace blink {
  namespace raster {
   
    template<class, class > class rectangle_window_view; // forward declaration

    template<typename Raster, typename IndicatorGenerator>
    class rectangle_window_iterator
    {
    public:
      using indicator = typename IndicatorGenerator::indicator;
    private:

      struct coordinate
      {
        coordinate(int row = 0, int col = 0) : m_row(row), m_col(col)
        {}
        int m_row;
        int m_col;
      };

      using coordinate_type = coordinate;
      using index_type = int;
      using indicator_functions = indicator_functions<indicator,
        typename traits<Raster>::value_type>;
      using rectangle_window_view = rectangle_window_view<Raster, IndicatorGenerator>;

    public:
      using value_type = indicator;
      using reference = const value_type&;
      using difference_type = void;
      using pointer = void;
      using iterator_category = std::input_iterator_tag;
      
      rectangle_window_iterator() = default;

      void find_end(const rectangle_window_view& view)
      {
        m_view = &view;
        m_coordinates = coordinate(m_view->rows(), 0);
      }

      void find_begin(const rectangle_window_view& view)
      {
        m_coordinates = coordinate(0, 0);
        m_view = &view;
        begin_buffer();
        begin_cell();
        begin_subrasters();
      }

      void begin_buffer()
      {
        int first_row = std::max(0, m_view->m_first_row - 
          m_view->m_rows_before);

        int end_row = std::min(m_view->m_raster.rows(), m_view->m_first_row + 
          m_view->m_rows_after + 1);

        int first_col = std::max(0, m_view->m_first_col - 
          m_view->m_cols_before);
        
        int end_col = std::min(m_view->m_raster.cols(), m_view->m_first_col + 
          m_view->cols() + m_view->m_cols_after);

        int buffersize = end_col - first_col;

        m_buffer.assign(buffersize, m_view->m_indicator_generator());
        
        auto frame = m_view->m_raster.sub_raster(
          first_row, first_col
          , end_row - first_row
          , end_col - first_col);

        auto iframe = frame.begin();

        for (int r = first_row; r < end_row; ++r) {
          auto ib = m_buffer.begin();
          for (int c = first_col; c < end_col; ++c, ++ib, ++iframe)  {
            indicator_functions::add(*ib, *iframe);
          }
        }
      }
 
      void begin_subrasters()
      {
        m_subtract_row_iterator = m_view->m_subtract_row_subraster.begin();
        m_add_row_iterator = m_view->m_add_row_subraster.begin();
        
        int leading_rows = std::min<int>(m_view->m_rows_before, 
          m_view->m_first_row);

        m_countdown_subtract_row = m_view->m_rows_before - leading_rows ;
        m_countdown_add_row = m_view->m_raster.rows() - m_view->m_first_row - 
          m_view->m_rows_after - 1;
      }

      void begin_cell()
      {
        m_indicator = m_view->m_indicator_generator();
        int leading_cols = std::min<int>(m_view->m_cols_before, 
          m_view->m_first_col);
        
        int end_col = std::min<int>(m_view->m_raster.cols(), 
          m_view->m_first_col + m_view->m_cols_after + 1);
        
        int n = std::min(leading_cols + m_view->m_cols_after + 1, 
          static_cast<int>(m_buffer.size()));
        
        auto ib = m_buffer.begin();
        for (int i = 0; i < n; ++i, ++ib) {
          m_indicator.add_subtotal(*ib);
        }
        
        m_countdown_subtract_col = m_view->m_cols_before - leading_cols;
        m_countdown_add_col = m_view->m_raster.cols() - m_view->m_first_col - 
          m_view->m_cols_after - 1;
        m_add_col_index = m_view->m_cols_after + 1 + leading_cols;
        m_subtract_col_index = 0;
      }

      rectangle_window_iterator& operator++()
      {
        if (++m_coordinates.m_col != m_view->cols()) {
          move_col();
        }
        else {
          m_coordinates.m_col = 0; // end of row, first new row
          if (++m_coordinates.m_row != m_view->rows()) {
            move_row();
          }
          // else: end of raster
        }
        return *this;
      }

      rectangle_window_iterator operator++(int)
      {
        rectangle_window_iterator copy = *this;
        ++(*this);
        return copy;
      }

      bool operator==(const rectangle_window_iterator& other) const
      {
        return m_coordinates.m_row == other.m_coordinates.m_row 
          &&   m_coordinates.m_col == other.m_coordinates.m_col;
      }
      
      bool operator!=(const rectangle_window_iterator& other) const
      {
        return m_coordinates.m_row != other.m_coordinates.m_row 
          ||   m_coordinates.m_col != other.m_coordinates.m_col;
      }

      reference operator*() const
      {
        return m_indicator;
      }

    private:
      void move_col()
      {
        if (m_countdown_add_col> 0) {
          add_col();
          --m_countdown_add_col;
        }
        if (m_countdown_subtract_col ==0) {
          subtract_col();
        }
        else {
          --m_countdown_subtract_col;
        }
      }

      void move_row()
      {
        if (m_countdown_add_row > 0) {
          add_row();
          --m_countdown_add_row;
        }
        if (m_countdown_subtract_row == 0) {
          subtract_row();
        }
        else {
          --m_countdown_subtract_row;
       }
        begin_cell();
      }
  
      void add_col()
      {
          m_indicator.add_subtotal(m_buffer[m_add_col_index]);
          ++m_add_col_index;
      }

      void subtract_col()
      {
        m_indicator.subtract_subtotal(m_buffer[m_subtract_col_index]);
        ++m_subtract_col_index;
      }
      
      void add_row()
      {
        auto i = m_buffer.begin();
        auto i_end = m_buffer.end();
        for (; i != i_end; ++i, ++m_add_row_iterator) {
          indicator_functions::add(*i, *m_add_row_iterator);
        }
      }
      
      void subtract_row()
      {
        auto i = m_buffer.begin();
        auto i_end = m_buffer.end();
        for (; i != i_end; ++i, ++m_subtract_row_iterator) {
          indicator_functions::subtract(*i, *m_subtract_row_iterator);
        }
      }
      using subraster_type = typename traits<Raster>::sub_raster;
      using input_iterator =
        typename blink::raster::traits<subraster_type>::const_iterator;

      coordinate m_coordinates;
      const rectangle_window_view* m_view;
      indicator m_indicator;
   
      int m_subtract_col_index;
      int m_add_col_index;
      
      input_iterator m_subtract_row_iterator;
      input_iterator m_add_row_iterator;

      int m_countdown_subtract_col;
      int m_countdown_add_col;
      int m_countdown_subtract_row;
      int m_countdown_add_row;

      // Stores one indicator for each column, may be problematic for large 
      // indicators / rasters
      std::vector<indicator> m_buffer;
    };

    template<class Raster, class IndicatorGenerator>
    class rectangle_window_view
    {
     public:
      using iterator = rectangle_window_iterator<Raster, IndicatorGenerator>;
     
      rectangle_window_view(Raster raster, int rows_before, int rows_after,
        int cols_before, int cols_after, const IndicatorGenerator& initializer)
        : m_raster(raster)
        , m_rows_before(rows_before), m_cols_before(cols_before)
        , m_rows_after(rows_after), m_cols_after(cols_after)
        , m_first_row(0), m_first_col(0)
        , m_rows(raster.rows()), m_cols(raster.cols()), m_indicator_generator(initializer)
      {
        make_subrasters();
      }

      rectangle_window_view() = default;

      iterator begin() const
      {
        iterator i;
        i.find_begin(*this);
        return i;
      }

      iterator end() const
      {
        iterator i;
        i.find_end(*this);
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

      rectangle_window_view sub_raster(int first_row, int first_col, 
        int rows, int cols) const
      {
        rectangle_window_view b(m_raster, m_rows_before, m_rows_after, 
          m_cols_before, m_cols_after, m_indicator_generator);

        b.m_first_row = m_first_row + first_row;
        b.m_first_col = m_first_col + first_col;
        b.m_rows = rows;
        b.m_cols = cols;

        assert(b.m_first_row + rows <= m_raster.rows());
        assert(b.m_first_col + cols <= m_raster.cols());
        
        b.make_subrasters();
        
        return b;
      }
    private:
      friend class rectangle_window_iterator<Raster, IndicatorGenerator>;

      void make_subrasters()
      {
        int first_col = std::max(m_first_col - m_cols_before, 0);
        int end_col = std::min(m_raster.cols(), m_first_col + cols() + 
          m_cols_after);
        int first_add_row = std::min(m_first_row + m_rows_after + 1, 
          m_raster.rows());
        int end_add_row = std::min(m_first_row + m_rows_after + rows(), 
          m_raster.rows());
        int first_subtract_row = std::max(m_first_row - m_rows_before, 0);
        int end_subtract_row = std::max(m_first_row - m_rows_before + rows(), 
          0);

        m_add_row_subraster = m_raster.sub_raster(
          first_add_row, first_col
          , end_add_row - first_add_row, end_col - first_col);

        m_subtract_row_subraster = m_raster.sub_raster(
          first_subtract_row, first_col
          , end_subtract_row - first_subtract_row, end_col - first_col);
      }

      using subraster_type = 
        typename blink::raster::traits<Raster>::sub_raster;
      
      Raster m_raster;
      subraster_type m_add_row_subraster;
      subraster_type m_subtract_row_subraster;
      
      IndicatorGenerator m_indicator_generator;
     
      int m_rows_before;
      int m_rows_after;
      int m_cols_before;
      int m_cols_after;

      int m_first_row;
      int m_first_col;
      int m_rows;
      int m_cols;
    };

    template<class Raster, class IndicatorGenerator>
    rectangle_window_view<Raster, IndicatorGenerator>
      make_rectangle_window_view(
        Raster raster, 
        int rows_before, int rows_after, int cols_before, int cols_after, 
        const IndicatorGenerator& initializer)
    {
      using return_type = rectangle_window_view<Raster, IndicatorGenerator>;
      return return_type(raster, rows_before, rows_after, cols_before, cols_after, initializer);
    }
  }  
}
