//example_open_any.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>

namespace pr = pronto::raster;

int main()
{
  // prepare a file in a separate scope 
  {
    auto raster = pr::create<int>("test.tif", 3, 4, GDT_Byte);
    auto i = 0;
    for (auto&& v : raster) {
      i = (i + 3) % 7;
      v = i;
    }
  }
  // open without specifying that it is an integer type or GDT_Byte 
  pr::any_blind_raster abr = pr::open_any("test.tif");
  plot_raster(abr);
  return 0;
}