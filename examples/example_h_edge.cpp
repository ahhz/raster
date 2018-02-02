//example_h_edge.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/edge_raster.h>

namespace pr = pronto::raster;

int main()
{
  auto raster = pr::create_temp<unsigned char>(3, 4);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }
  
  auto edges = pr::h_edge(raster);
 
  plot_raster(raster);
  plot_raster(edges);
  
  return 0;
}