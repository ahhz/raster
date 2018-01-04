//example_h_edge.cpp

#include <blink/raster/io.h>
#include <blink/raster/plot_raster.h>
#include <blink/raster/edge_raster.h>

namespace br = blink::raster;

int main()
{
  auto raster = br::create_temp<unsigned char>(3, 4);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }
  
  auto edges = br::h_edge(raster);
 
  plot_raster(raster);
  plot_raster(edges);
  
  return 0;
}