//example_offset.cpp
#include <pronto/raster/distance_transform.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/io.h>

namespace pr = pronto::raster;

int main()
{
  auto raster = pr::create_temp<int>(5, 6);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }
  
  auto distance = pr::create_temp<double>(5, 6);
  pr::euclidean_distance_transform(raster, distance, 1);
  pr::plot_raster(raster); 
  pr::plot_raster(distance);
  return 0;
}