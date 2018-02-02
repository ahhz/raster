//example_uniform.cpp

#include <pronto/raster/uniform_raster_view.h>
#include <pronto/raster/plot_raster.h>

namespace br = pronto::raster;

int main()
{
  auto raster = br::uniform(5, 6, 3.14);
  plot_raster(raster);
  return 0;
}