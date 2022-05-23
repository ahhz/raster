//
//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once

#include <pronto/raster/edge_raster.h>
#include <pronto/raster/indicator_functions.h>
#include <pronto/raster/offset_raster_view.h>
#include <pronto/raster/optional_raster_view.h>
#include <pronto/raster/padded_raster_view.h>

#include <cmath> // sqrt
#include <vector>

namespace pronto {
  namespace raster {

    template<class, class>
    class circular_edge_window_view; // forward declaration

    template<class Raster, class IndicatorGenerator>
    class circular_edge_window_iterator
    {
    public:
      using indicator = typename IndicatorGenerator::indicator;

    private:
      using indicator = typename IndicatorGenerator::indicator;
      using view_type = circular_edge_window_view<Raster, IndicatorGenerator>;
      using padded_edges_view =
        padded_raster_view <
        optional_raster_view <
        edge_raster_view <
        Raster> > >;

      using sub_padded_edges_view
        = typename traits<padded_edges_view>::sub_raster;
      
      using sub_padded_iterator = typename traits<sub_padded_edges_view>::iterator;

      using indicator_functions = indicator_functions<indicator,
        typename traits<sub_padded_edges_view>::value_type>;

    public:
      using value_type = indicator;
      using reference = const indicator&;
      using difference_type = void;
      using pointer = void;
      using iterator_category = std::input_iterator_tag;

      reference operator*() const
      {
        return m_current;
      }
     
      circular_edge_window_iterator& operator++()
      {
        increment();
        return *this;
      }

      circular_edge_window_iterator operator++(int)
      {
        circular_edge_window_iterator copy(*this);
        increment();
        return copy;
      }

      bool operator==(const circular_edge_window_iterator& that) const
      {
        return is_equal(that);
      }

      bool operator!=(const circular_edge_window_iterator& that) const
      {
        return !is_equal(that);
      }
     
    private:
      friend class view_type;
      
      void find_begin(const view_type& view)
      {
        m_view = &view;
        m_col = 0;
        m_row = 0;
        m_current = m_view->m_indicator_generator();
        m_start_of_row = m_view->m_indicator_generator();
        for (auto&& i : m_view->m_init_v_ranges) {
          for (auto&& j : i) {
            indicator_functions::add(m_current, j);
          }
        }

        for (auto&& i : m_view->m_init_h_ranges) {
          for (auto&& j : i) {
            indicator_functions::add(m_current, j);
          }
        }

        m_start_of_row = m_current;
        clear_iterators();

        for (auto&& i : m_view->m_add_right_v_ranges) {
          m_add_right_v_iterators.push_back(i.begin());
        }

        for (auto&& i : m_view->m_subtract_left_v_ranges) {
          m_subtract_left_v_iterators.push_back(i.begin());
        }

        for (auto&& i : m_view->m_add_bottom_v_ranges) {
          m_add_bottom_v_iterators.push_back(i.begin());
        }
        
        for (auto&& i : m_view->m_subtract_top_v_ranges) {
          m_subtract_top_v_iterators.push_back(i.begin());
        }

        for (auto&& i : m_view->m_add_right_h_ranges) {
          m_add_right_h_iterators.push_back(i.begin());
        }

        for (auto&& i : m_view->m_subtract_left_h_ranges) {
          m_subtract_left_h_iterators.push_back(i.begin());
        }

        for (auto&& i : m_view->m_add_bottom_h_ranges) {
          m_add_bottom_h_iterators.push_back(i.begin());
        }

        for (auto&& i : m_view->m_subtract_top_h_ranges) {
          m_subtract_top_h_iterators.push_back(i.begin());
        }
      }

      void find_end(const view_type& view)
      {
        m_view = &view;
        m_col = 0;
        m_row = m_view->rows();
        clear_iterators();
      }

    private:
      void clear_iterators()
      {
        m_add_right_v_iterators.clear();
        m_subtract_left_v_iterators.clear();
        m_add_bottom_v_iterators.clear();
        m_subtract_top_v_iterators.clear();

        m_add_right_h_iterators.clear();
        m_subtract_left_h_iterators.clear();
        m_add_bottom_h_iterators.clear();
        m_subtract_top_h_iterators.clear();
      }

      void increment()
      {
        if (++m_col != m_view->cols()) {
          for (auto&& i : m_add_right_v_iterators) {
            indicator_functions::add(m_current, *i);
            ++i;
          }
          for (auto&& i : m_subtract_left_v_iterators) {
            indicator_functions::subtract(m_current, *i);
            ++i;
          }
          for (auto&& i : m_add_right_h_iterators) {
            indicator_functions::add(m_current, *i);
            ++i;
          }
          for (auto&& i : m_subtract_left_h_iterators) {
            indicator_functions::subtract(m_current, *i);
            ++i;
          }
        }
        else if (++m_row != m_view->rows()) {
          m_col = 0;
          for (auto&& i : m_add_bottom_v_iterators) {
            indicator_functions::add(m_start_of_row, *i);
            ++i;
          }
          for (auto&& i : m_subtract_top_v_iterators) {
            indicator_functions::subtract(m_start_of_row, *i);
            ++i;
          }
          for (auto&& i : m_add_bottom_h_iterators) {
            indicator_functions::add(m_start_of_row, *i);
            ++i;
          }
          for (auto&& i : m_subtract_top_h_iterators) {
            indicator_functions::subtract(m_start_of_row, *i);
            ++i;
          }

          m_current = m_start_of_row;
        }
        else { // end of raster
          m_col = 0;
          clear_iterators();

        }
      }
      bool is_equal(const circular_edge_window_iterator& that) const
      {
        return that.m_row == m_row && that.m_col == m_col;
      }

      indicator m_start_of_row;
      indicator m_current;

      std::vector<sub_padded_iterator> m_add_right_v_iterators;
      std::vector<sub_padded_iterator> m_subtract_left_v_iterators;
      std::vector<sub_padded_iterator> m_add_bottom_v_iterators;
      std::vector<sub_padded_iterator> m_subtract_top_v_iterators;

      std::vector<sub_padded_iterator> m_add_right_h_iterators;
      std::vector<sub_padded_iterator> m_subtract_left_h_iterators;
      std::vector<sub_padded_iterator> m_add_bottom_h_iterators;
      std::vector<sub_padded_iterator> m_subtract_top_h_iterators;

      int m_row;
      int m_col;
      const view_type* m_view;
    };

    template<class Raster, class IndicatorGenerator>
    class circular_edge_window_view
    {
    private:
      using padded_edge_view = 
        padded_raster_view < 
        optional_raster_view < 
        edge_raster_view < 
        Raster> > >;
    
      using sub_padded_edge_view 
        = typename traits<padded_edge_view>::sub_raster;

      // This private constructor is used by sub_raster()  
      circular_edge_window_view(Raster raster, double radius
        , IndicatorGenerator initializer
        , int first_row, int first_col, int rows, int cols)
        : m_raster(raster), m_radius(radius)
        , m_indicator_generator(initializer)
        , m_first_row(first_row), m_first_col(first_col)
        , m_rows(rows), m_cols(cols)
      {
        assert(raster.rows() >= m_first_row + m_rows);
        assert(raster.cols() >= m_first_col + m_cols);
        init_views();
      }

    public:
      using iterator = circular_edge_window_iterator<Raster, IndicatorGenerator>;
    
      circular_edge_window_view(Raster raster, double radius
        , IndicatorGenerator indicator_generator)
        : m_raster(raster), m_radius(radius)
        , m_indicator_generator(indicator_generator)
        , m_first_row(0), m_first_col(0)
        , m_rows(raster.rows()), m_cols(raster.cols())
      {
        init_views();
      }
      
      circular_edge_window_view() = default;
      circular_edge_window_view(const circular_edge_window_view&) = default;
      circular_edge_window_view& operator=(const circular_edge_window_view&) = default;
      circular_edge_window_view& operator=(circular_edge_window_view&&) = default;

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

      circular_edge_window_view sub_raster(int first_row, int first_col, int rows,
        int cols) const
      {
        return circular_edge_window_view(m_raster, m_radius, m_indicator_generator,
          m_first_row + first_row, m_first_col + first_col, rows, cols);
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
        return m_rows * m_cols;
      }

     private:
      friend class circular_edge_window_iterator<Raster, IndicatorGenerator>;

      // ranges for h_edges and v_edges
      std::vector<sub_padded_edge_view> m_init_v_ranges;
      std::vector<sub_padded_edge_view> m_add_right_v_ranges;
      std::vector<sub_padded_edge_view> m_subtract_left_v_ranges;
      std::vector<sub_padded_edge_view> m_add_bottom_v_ranges;
      std::vector<sub_padded_edge_view> m_subtract_top_v_ranges;

      std::vector<sub_padded_edge_view> m_init_h_ranges;
      std::vector<sub_padded_edge_view> m_add_right_h_ranges;
      std::vector<sub_padded_edge_view> m_subtract_left_h_ranges;
      std::vector<sub_padded_edge_view> m_add_bottom_h_ranges;
      std::vector<sub_padded_edge_view> m_subtract_top_h_ranges;

    private:
      void init_views()
      {
        m_init_v_ranges.clear();
        m_add_right_v_ranges.clear();
        m_subtract_left_v_ranges.clear();
        m_add_bottom_v_ranges.clear();
        m_subtract_top_v_ranges.clear();

        m_init_h_ranges.clear();
        m_add_right_h_ranges.clear();
        m_subtract_left_h_ranges.clear();
        m_add_bottom_h_ranges.clear();
        m_subtract_top_h_ranges.clear();

        double sqr_radius = m_radius * m_radius;

        int radius_round = static_cast<int>(m_radius);
        int sqr_radius_round = static_cast<int>(sqr_radius);
               
        padded_edge_view h_padded = pad(optionalize(h_edge(m_raster) )
          , radius_round, radius_round + 1, radius_round, radius_round + 1
          , std::nullopt);
        // + 1 makes the next step easier because it means that the end of 
        // a stretch over a row is still on the same row and not the first 
        // element of the next row
       

        padded_edge_view v_padded = pad(optionalize(v_edge(m_raster))
          , radius_round, radius_round + 1, radius_round, radius_round + 1
          , std::nullopt);
        // + 1 makes the next step easier because it means that the end of 
        // a stretch over a row is still on the same row and not the first 
        // element of the next row

        for (int i = -radius_round; i <= radius_round; ++i) {
       
          const int j = static_cast<int>(std::sqrt(sqr_radius - i * i));
          
          if (j > 0) {
            m_init_v_ranges.push_back(v_padded.sub_raster(
              radius_round + m_first_row + i, 
              radius_round + m_first_col - j, 1, 2 * j));

            m_add_right_v_ranges.push_back(v_padded.sub_raster(
              radius_round + m_first_row + i, 
              radius_round + m_first_col + j, m_rows, m_cols - 1));
            // -1 because the step to a next row does not need adding

            m_subtract_left_v_ranges.push_back(v_padded.sub_raster(
              radius_round + m_first_row + i, 
              radius_round + m_first_col - j, m_rows, m_cols - 1));
            // -1 because the step to a next row does not need subtracting

      

          }
          if (i != radius_round) {
            int ri = (i >= 0 ? std::abs(i) + 1 : std::abs(i));
            int rj = static_cast<int>(std::sqrt(sqr_radius - ri * ri));

            m_init_h_ranges.push_back(h_padded.sub_raster(
              radius_round + m_first_row + i, 
              radius_round + m_first_col - rj, 1, 2 * rj + 1));

            m_add_right_h_ranges.push_back(h_padded.sub_raster(
              radius_round + m_first_row + i, 
              radius_round + m_first_col + rj + 1, m_rows, m_cols - 1));
            // -1 because the step to a next row does not need adding

            m_subtract_left_h_ranges.push_back(h_padded.sub_raster(
              radius_round + m_first_row + i, 
              radius_round + m_first_col - rj, m_rows, m_cols - 1));
            // -1 because the step to a next row does not need subtracting

            m_add_bottom_v_ranges.push_back(v_padded.sub_raster(
              radius_round + m_first_row + rj + 1,
              radius_round + m_first_col + i, m_rows, 1));

            m_subtract_top_v_ranges.push_back(v_padded.sub_raster(
              radius_round + m_first_row - rj,
              radius_round + m_first_col + i, m_rows, 1));
          }

          if (j > 0) {
            m_add_bottom_h_ranges.push_back(h_padded.sub_raster(
              radius_round + m_first_row + j,
              radius_round + m_first_col + i, m_rows, 1));

            m_subtract_top_h_ranges.push_back(h_padded.sub_raster(
              radius_round + m_first_row - j, 
              radius_round + m_first_col + i, m_rows, 1));
          }
        }
      }

      IndicatorGenerator m_indicator_generator;

      double m_radius;
      Raster m_raster;
      int m_first_row;
      int m_first_col;
      int m_rows;
      int m_cols;
    };

    template<class Raster, class IndicatorGenerator>
    circular_edge_window_view<Raster, IndicatorGenerator>
      make_circular_edge_window_view(
        Raster raster,
        double radius,
        IndicatorGenerator indicator_generator)
    {
      using return_type = circular_edge_window_view<Raster, IndicatorGenerator>;
      return return_type(raster, radius, indicator_generator);
    }
 
  } 
} 
