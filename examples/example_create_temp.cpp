//example_create_temp.cpp

#include <blink/raster/io.h>
#include <blink/raster/plot_raster.h>

namespace br = blink::raster;

int main()
{
  auto raster = br::create_temp<int>(3, 4, GDT_Byte);
  int i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }
  plot_raster(raster);
  return 0;
}