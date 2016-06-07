###############################################################################
#
# Require 3.0 for INTERFACE projects

cmake_minimum_required (VERSION 3.0)

###############################################################################
#
# Resolve GDAL dependency
#

find_package(GDAL 1.10.0 REQUIRED)
mark_as_advanced(CLEAR GDAL_INCLUDE_DIR) 
mark_as_advanced(CLEAR GDAL_LIBRARY) 

add_library(gdal_interface INTERFACE)
target_include_directories(gdal_interface INTERFACE ${GDAL_INCLUDE_DIR})
target_link_libraries(gdal_interface INTERFACE ${GDAL_LIBRARY} )

###############################################################################
#
# Resolve BOOST dependency
#

set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost 1.55.0 REQUIRED COMPONENTS system filesystem)
mark_as_advanced(CLEAR Boost_INCLUDE_DIR) 
mark_as_advanced(CLEAR Boost_LIBRARY_DIR) 

add_library(boost_interface INTERFACE)
target_include_directories(boost_interface INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(boost_interface INTERFACE ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries(boost_interface INTERFACE ${Boost_SYSTEM_LIBRARY})


###############################################################################
#
# Create STATIC library
#

add_library(raster STATIC "source/gdal_raster_lru.cpp")
target_include_directories(raster PUBLIC "include/")
target_link_libraries(raster PUBLIC gdal_interface)
target_link_libraries(raster PUBLIC boost_interface)
target_link_libraries(raster PUBLIC iterator)

###############################################################################
#
# Create executable using STATIC library
#

add_executable(raster_demo main.cpp)
target_link_libraries(raster_demo PRIVATE raster)



