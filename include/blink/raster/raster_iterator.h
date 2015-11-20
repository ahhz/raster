//
//=======================================================================
// Copyright 2015
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// raster iterator tags are used to inspect the properties of a raster iterator
// These are:
//     row_major / col_major
//     pixel, h_edge, v_edge, h_edge_first_only, h_edge_second_only
//     read_only / read_write
//     
#ifndef BLINK_RASTER_ITERATOR_H_AHZ
#define BLINK_RASTER_ITERATOR_H_AHZ

namespace blink {
  namespace raster {

    //namespace raster_iterator_tag
    //{
      namespace orientation
      {
        struct row_major{};
        struct col_major{};
      }

      namespace element
      {
        struct pixel{};
        struct h_edge{};
        struct h_edge_first_only{};// first is top
        struct h_edge_second_only{};
        struct v_edge{};
        struct v_edge_first_only{}; //first is left
        struct v_edge_second_only{};
      }

      namespace access
      {
        struct read_only{};
        struct read_write{};
      }
    }
  //}
}
#endif