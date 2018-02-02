//example_pad.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/padded_raster_view.h>
#include <pronto/raster/plot_raster.h>

namespace pr = pronto::raster;

int main()
{
  auto raster = pr::create_temp<int>(3, 4);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }
  
  auto padded = pr::pad(raster, 1, 4, 3, 2,-1);
  plot_raster(padded);
  return 0;
}