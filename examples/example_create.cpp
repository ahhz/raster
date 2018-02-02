//example_create.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>

namespace pr = pronto::raster;

int main()
{
  auto raster = pr::create<int>("test.tif", 3, 4, GDT_Byte);
  int i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }
  plot_raster(raster);
  return 0;
}