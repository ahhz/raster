//
//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================

// TODO: When a GDALRasterBand is closed, now its parent GDALDataSet is 
// closed. This will be problematic when multiple bands from the same 
// dataset are used.  So for now we only support one band per 
// dataset.  

#pragma once
#include <pronto/raster/any_blind_raster.h>
#include <pronto/raster/assign.h>
#include <pronto/raster/blind_function.h>
#include <pronto/raster/exceptions.h>
#include <pronto/raster/filesystem.h>
#include <pronto/raster/gdal_includes.h>
#include <pronto/raster/gdal_raster_view.h>
#include <pronto/raster/optional.h>

#include <iostream>

namespace pronto
{
  namespace raster
  {
    namespace detail {
      template<typename T> struct native_gdal_data_type
      {
        static const GDALDataType value = GDT_Unknown;
      };

      template<> struct native_gdal_data_type<int32_t>
      {
        static const GDALDataType value = GDT_Int32;
      };

      template<>
      struct native_gdal_data_type<uint32_t>
      {
        static const GDALDataType value = GDT_UInt32;
      };

      template<> struct native_gdal_data_type<int16_t>
      {
        static const GDALDataType value = GDT_Int16;
      };

      template<> struct native_gdal_data_type<uint16_t>
      {
        static const GDALDataType value = GDT_UInt16;
      };

      template<> struct native_gdal_data_type<uint8_t>
      {
        static const GDALDataType value = GDT_Byte;
      };

      template<> struct native_gdal_data_type<bool>
      {
        static const GDALDataType value = GDT_Byte;
      };

      template<> struct native_gdal_data_type<float>
      {
        static const GDALDataType value = GDT_Float32;
      };

      template<> struct native_gdal_data_type<double>
      {
        static const GDALDataType value = GDT_Float64;
      };
      /*
          template<> struct native_gdal_data_type<cint16_t>
          {
            static const GDALDataType value = GDT_CInt16;
          };

          template<> struct native_gdal_data_type<cint32_t>
          {
            static const GDALDataType value = GDT_CInt32;
          };
          template<> struct native_gdal_data_type<cfloat32_t>
          {
            static const GDALDataType value = GDT_CFloat32;
          };
          template<> struct native_gdal_data_type<cfloat64_t>
          {
            static const GDALDataType value = GDT_CFloat64;
          };

    */
    }
  }
}
