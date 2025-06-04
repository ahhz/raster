//
//=======================================================================
// Copyright 2016-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once

// Suppress MSVC warning C4251: 'identifier' : class 'type' needs to have dll-interface
// This warning is triggered because GDAL uses STL types in its API, and when building
// or consuming DLLs on Windows, MSVC expects all exported types to have a DLL interface.
// GDAL is widely used and safe in this context, and this warning can be safely ignored
// for GDAL headers. Suppressing it here prevents unnecessary build noise.

#pragma warning( push )
#pragma warning( disable : 4251 )
#include <cpl_string.h> // part of GDAL
#include <gdal.h>       // part of GDAL
#include <gdal_priv.h>  // part of GDAL
#pragma warning( pop ) 
