//example_coordiante_raster_view.cpp

// This example show how you can access coordinates when iterating over a raster
#include <pronto/raster/coordinate_raster_view.h>
#include <pronto/raster/uniform_raster_view.h>
#include <pronto/raster/tuple_raster_view.h>

#include <iostream>
namespace pr = pronto::raster;

int main()
{
  auto cr = pr::coordinate_raster(5, 8);
  auto pi = pr::uniform(5, 8, 3.1427);
  auto zip = pr::raster_tuple(pi, cr);
  
  for (auto&& z : zip.sub_raster(3,5,2,4)) {
    std::cout
      << "row: "    << std::get<1>(z).first  << ", "
      << "column: " << std::get<1>(z).second << ", "
      << "value: "  << std::get<0>(z) 
      << std::endl;
  }
  return 0;
}