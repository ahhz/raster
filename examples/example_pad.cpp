//example_pad.cpp

#include <blink/raster/io.h>
#include <blink/raster/padded_raster_view.h>
#include <blink/raster/plot_raster.h>

namespace br = blink::raster;

int main()
{
  auto raster = br::create_temp<int>(3, 4);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }
  
  auto padded = br::pad(raster, 1, 4, 3, 2,-1);
  plot_raster(padded);
  return 0;
}