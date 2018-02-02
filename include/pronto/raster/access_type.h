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

#include <pronto/raster/gdal_includes.h>

namespace pronto
{
  namespace raster
  {
    enum access
    {
      read_only,
      read_write
    };
    
    namespace access_type
    {
      using read_only_t = std::integral_constant<access, read_only>;
      using read_write_t = std::integral_constant<access, read_write>; 
    }

    static GDALAccess gdal_access(access a)
    {
      switch (a) {
      case read_only: return GA_ReadOnly;
      case read_write: return GA_Update;
      default:return GA_Update;
      }
    }
  }
}
