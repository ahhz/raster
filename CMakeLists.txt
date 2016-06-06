cmake_minimum_required (VERSION 3.0)



#file(GLOB_RECURSE MY_HDRS "*.h")
#add_library(raster "source/gdal_raster_lru.cpp" ${MY_HDRS})
add_library(raster STATIC "source/gdal_raster_lru.cpp")

# make sure that downstream projects inherit the include files of raster
target_include_directories(raster PUBLIC "include/")

#target_include_directories(raster INTERFACE 
#  "include/" $<INSTALL_INTERFACE:include/raster>
#)
add_executable(raster_demo main.cpp)

target_link_libraries(raster_demo PUBLIC iterator)
target_link_libraries(raster_demo PUBLIC raster)
#target_link_libraries(raster_demo PUBLIC ${raster_LIBRARIES})
###############################################################################
#
# Search for dependencies
#
# Based on https://github.com/libLAS/libLAS/blob/master/CMakeLists.txt
# Because it requires both GDAL and BOOST

# Boost C++ Libraries support - required

# Default to using static, multithreaded libraries for 
# linking under MSVC.

set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost COMPONENTS system filesystem REQUIRED)
mark_as_advanced(CLEAR Boost_INCLUDE_DIR) 
mark_as_advanced(CLEAR Boost_LIBRARY_DIR) 

target_link_libraries(raster_demo 
  PUBLIC ${Boost_FILESYSTEM_LIBRARY}
  PUBLIC ${Boost_SYSTEM_LIBRARY}
)

target_link_libraries(raster 
  PUBLIC ${Boost_FILESYSTEM_LIBRARY}
  PUBLIC ${Boost_SYSTEM_LIBRARY}
)

include_directories(${Boost_INCLUDE_DIRS})

find_package(GDAL 1.10.0)
include_directories(${GDAL_INCLUDE_DIR})
mark_as_advanced(CLEAR GDAL_INCLUDE_DIR) 
mark_as_advanced(CLEAR GDAL_LIBRARY) 
target_link_libraries(raster_demo 
   PUBLIC ${GDAL_LIBRARY}
)
target_link_libraries(raster
   PUBLIC ${GDAL_LIBRARY}
)
###############################################################################