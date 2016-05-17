//
//=======================================================================
// Copyright 2015
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// Defines exception to be thrown by the moving_window library.
//

#ifndef BLINK_RASTER_EXCEPTIONS_H_AHZ
#define BLINK_RASTER_EXCEPTIONS_H_AHZ

#include <boost/exception/all.hpp>

namespace blink {
  namespace raster {
    struct assigning_an_uninitialized_optional_to_an_initialized_iterator : public boost::exception, public std::exception
    {
      const char *what() const _NOEXCEPT { return
        "assigning an uninitialized optional to an initialized_iterator"; }
    };
    struct assigning_an_initialized_optional_to_an_uninitialized_iterator : public boost::exception, public std::exception
    {
      const char *what() const{
        return
          "assigning an initialized optional to an uninitialized_iterator";
      }
    };

    struct creating_a_raster_failed : public boost::exception, public std::exception
    {
      const char *what() const { return "creating a raster failed"; }
    };

    struct insufficient_memory_for_raster_block : public boost::exception, public std::exception
    {
      const char *what() const { return "insufficient memory for reading a raster block"; }
    };

    struct opening_raster_failed : public boost::exception, public std::exception
    {
      const char *what() const { return "opening raster failed"; }
    };
    struct reading_from_raster_failed : public boost::exception, public std::exception
    {
      const char *what() const { return "reading from raster failed"; }
    };

    struct writing_to_raster_failed : public boost::exception, public std::exception
    {
      const char *what() const { return "writing to raster failed"; }
    };

  }
}
#endif