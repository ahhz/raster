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

#include <pronto/raster/io.h>

namespace pronto
{
  namespace raster
  {
    class gdal_raster_view_allocator
    {
    public:
      template<class T> 
      using type = gdal_raster_view<T>;

      template<class T>
      gdal_raster_view<T> allocate(int rows, int cols)
      {
        return create_temp<T>(rows, cols);
      }
    };

    using default_raster_allocator = gdal_raster_view_allocator;
  }
}
