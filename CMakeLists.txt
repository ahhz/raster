cmake_minimum_required (VERSION 2.8)
project(raster)
include_directories("include/")
include_directories("../iterator/include/")
file(GLOB_RECURSE MY_HDRS "*.h")
add_executable(raster_demo main.cpp)
add_library(raster STATIC dummy.cpp ${MY_HDRS})

###############################################################################
#
# Search for dependencies
#
# Based on https://github.com/libLAS/libLAS/blob/master/CMakeLists.txt
# Because it requires both GDAL and BOOST

# Boost C++ Libraries support - required
message(STATUS "Searching for Boost 1.55+ - done")

# Default to using static, multithreaded libraries for 
# linking under MSVC.

set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost COMPONENTS system filesystem REQUIRED)

target_link_libraries(raster_demo
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
)

include_directories(${Boost_INCLUDE_DIRS})

find_package(GDAL 1.10.0)
include_directories(${GDAL_INCLUDE_DIR})
mark_as_advanced(CLEAR GDAL_INCLUDE_DIR) 
mark_as_advanced(CLEAR GDAL_LIBRARY) 
target_link_libraries(raster_demo
      ${GDAL_LIBRARY}
)
###############################################################################