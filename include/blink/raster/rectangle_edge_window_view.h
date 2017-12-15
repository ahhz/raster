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

#include <blink/raster/edge_raster.h>
#include <blink/raster/indicator_functions.h>
#include <blink/raster/optional_raster_view.h>
#include <blink/raster/padded_raster_view.h>
#include <blink/raster/rectangle_window_view.h>

namespace blink {
  namespace raster {

    template<class Raster, class IndicatorGenerator>
    struct rectangle_edge_window_view
    {

    private:
      using edge_raster_type_helper_1 = edge_raster_view<Raster>;
      using edge_raster_type_helper_2 
        = optional_raster_view<edge_raster_type_helper_1>;
      using edge_raster_type = padded_raster_view<edge_raster_type_helper_2>;

      using single_window_view = rectangle_window_view<edge_raster_type, 
        IndicatorGenerator>;
      using combined_window_view = transform_raster_view<indicator_joiner
        , single_window_view, single_window_view>;
    public:
      using indicator = typename IndicatorGenerator::indicator;

      using iterator = typename traits<combined_window_view>::const_iterator;

      iterator begin() const
      {
        return m_combined.begin();
      }
      iterator end() const
      {
        return m_combined.end();
      }

      int rows() const
      {
        return m_combined.rows();
      }

      int cols() const
      {
        return m_combined.cols();
      }

      auto sub_raster(int first_row, int first_col, int rows, int cols) const 
        ->decltype(std::declval<combined_window_view>().sub_raster(first_row, first_col, rows, cols))
      {
        return m_combined.sub_raster(first_row, first_col, rows, cols);
      }
      
      // Separately tallying the vertical and horizontal edges and subsequently
      // joining them using the indicator_joiner is not necessarily the most
      // efficient, because it requires copying of indicators and iterating 
      // over their state variable, which can be expensive (but typically is 
      // not). 
      // It would be probalby be better to create and class for 
      // edge_rectangle_window_iterator that holds the indicator, and let both 
      // the h_edge iterator and the v_edge iterator add and subtract from 
      // that. This will be more cumbersome to implement and for now is 
      // considered premature optimisation.
      
      rectangle_edge_window_view(const Raster& raster
        , int rows_before, int rows_after, int cols_before, int cols_after
        , IndicatorGenerator indicator_generator)
        : m_indicator_generator(indicator_generator)
      {
        m_h_edges = pad(optionalize(h_edge(raster)), 0, 1, 0, 0, none);
        m_v_edges = pad(optionalize(v_edge(raster)), 0, 0, 0, 1, none);
        m_h_window = single_window_view(m_h_edges, rows_before, rows_after - 1
          , cols_before, cols_after, m_indicator_generator);
        m_v_window = single_window_view(m_v_edges, rows_before, rows_after
          , cols_before, cols_after - 1, m_indicator_generator);
        m_combined = transform(indicator_joiner{}, m_h_window, m_v_window);
      }

      // the four intermediate rasters are not necessary to keep, but leaving 
      // it now for debugging
      edge_raster_type m_h_edges;// not necessary to keep
      edge_raster_type m_v_edges;// not necessary to keep 
      single_window_view m_h_window;// not necessary to keep
      single_window_view m_v_window;// not necessary to keep
     
      // this is the only view required
      combined_window_view m_combined;

      IndicatorGenerator m_indicator_generator;
    };

    template<class Raster, class IndicatorGenerator>
    rectangle_edge_window_view<Raster, IndicatorGenerator>
      make_rectangle_edge_window_view(Raster raster
        , int rows_before, int rows_after, int cols_before, int cols_after
        , IndicatorGenerator m_indicator_generator)
    {
      using return_type = rectangle_window_view<Raster, IndicatorGenerator>;
      return return_type(raster, rows_before, rows_after, cols_before, cols_after, m_indicator_generator);
    }

    template<class Raster, class IndicatorGenerator>
    rectangle_edge_window_view<Raster, IndicatorGenerator>
      make_square_edge_window_view(Raster raster, int radius
        , IndicatorGenerator m_indicator_generator)
    {
      using return_type = rectangle_edge_window_view<Raster, IndicatorGenerator>;
      return return_type(raster, radius, radius, radius, radius, m_indicator_generator);
    }
  }
}
