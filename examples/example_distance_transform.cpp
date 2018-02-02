//example_offset.cpp
#include <pronto/raster/distance_transform.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/io.h>

namespace br = pronto::raster;

int main()
{
  auto raster = br::create_temp<int>(5, 6);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }
  
  auto distance = br::create_temp<double>(5, 6);
  br::euclidean_distance_transform(raster, distance, 1);
  br::plot_raster(raster); 
  br::plot_raster(distance);
  return 0;
}