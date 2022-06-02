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
    enum class access
    {
      read_only,
      read_write
    };
    
    enum class iteration_type
    {
      single_pass, 
      multi_pass
    };

    static GDALAccess gdal_access(access a)
    {
      switch (a) {
      case access::read_only: return GA_ReadOnly;
      case access::read_write: return GA_Update;
      default:return GA_Update;
      }
    }
  }
}
