//example_open_any.cpp

#include <blink/raster/io.h>
#include <blink/raster/plot_raster.h>

namespace br = blink::raster;

int main()
{
  // prepare a file in a separate scope 
  {
    auto raster = br::create<int>("test.tif", 3, 4, GDT_Byte);
    auto i = 0;
    for (auto&& v : raster) {
      i = (i + 3) % 7;
      v = i;
    }
  }
  // open without specifying that it is an integer type or GDT_Byte 
  br::any_blind_raster abr = br::open_any("test.tif");
  plot_raster(abr);
  return 0;
}