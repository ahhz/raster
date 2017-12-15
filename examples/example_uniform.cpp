//example_uniform.cpp

#include <blink/raster/uniform_raster_view.h>
#include <blink/raster/plot_raster.h>

namespace br = blink::raster;

int main()
{
  auto raster = br::uniform(5, 6, 3.14);
  plot_raster(raster);
  return 0;
}