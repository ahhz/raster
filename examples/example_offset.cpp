//example_offset.cpp

#include <blink/raster/io.h>
#include <blink/raster/offset_raster_view.h>
#include <blink/raster/plot_raster.h>

namespace br = blink::raster;

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