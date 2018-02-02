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

#include <exception>

namespace pronto {
  namespace raster {

    struct unsupported_gdal_datatype : public std::exception
    {
      const char *what() const noexcept {
        return
          "the gdal datatype is not supported";
      }
    };
    struct assigning_an_uninitialized_optional_to_an_initialized_iterator :  public std::exception
    {
      const char *what() const noexcept {
        return
          "assigning an uninitialized optional to an initialized_iterator";
      }
    };
    struct assigning_an_initialized_optional_to_an_uninitialized_iterator : public std::exception
    {
      const char *what() const noexcept {
        return
          "assigning an initialized optional to an uninitialized_iterator";
      }
    };

    struct creating_a_raster_failed : public std::exception
    {
      const char *what() const noexcept { return "creating a raster failed"; }
    };

    struct insufficient_memory_for_raster_block : public std::exception
    {
      const char *what() const noexcept { return "insufficient memory for reading a raster block"; }
    };

    struct opening_raster_failed : public std::exception
    {
      const char *what() const noexcept { return "opening raster failed"; }
    };
    struct reading_from_raster_failed : public std::exception
    {
      const char *what() const noexcept { return "reading from raster failed"; }
    };

    struct writing_to_raster_failed : public std::exception
    {
      const char *what() const noexcept { return "writing to raster failed"; }
    };

    struct closing_raster_failed : public std::exception
    {
      const char *what() const noexcept { return "closing raster failed"; }
    };

    struct deleting_raster_failed : public std::exception
    {
      const char *what() const noexcept { return "deleting raster failed"; }
    };

  }
}