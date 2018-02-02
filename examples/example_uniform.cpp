//example_uniform.cpp

#include <pronto/raster/uniform_raster_view.h>
#include <pronto/raster/plot_raster.h>

namespace pr = pronto::raster;

int main()
{
  auto raster = pr::uniform(5, 6, 3.14);
  plot_raster(raster);
  return 0;
}