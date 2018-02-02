//example_offset.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/offset_raster_view.h>
#include <pronto/raster/plot_raster.h>

namespace br = pronto::raster;

int main()
{
  auto raster = br::create_temp<int>(5, 6);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }
  
  auto off = br::offset(raster, 2, 3,-1);
  plot_raster(raster);
  plot_raster(off);
  return 0;
}