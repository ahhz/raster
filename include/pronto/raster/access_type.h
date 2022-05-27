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
    
    namespace access_type
    {
      using read_only_t = std::integral_constant<access, access::read_only>;
      using read_write_t = std::integral_constant<access, access::read_write>;
    }

    static GDALAccess gdal_access(access a)
    {
      switch (a) {
      case access::read_only: return GA_ReadOnly;
      case access::read_write: return GA_Update;
      default:return GA_Update;
      }
    }

    class single_pass {};
    class multi_pass {};
  }
}
