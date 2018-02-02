//example_open.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>

namespace pr = pronto::raster;

int main()
{
  {
    auto raster = pr::create<int>("test.tif", 3, 4, GDT_Byte);
  }
  auto opened_raster = pr::open<short>("test.tif");
  int i = 0;
  for (auto&& v : opened_raster) {
    i = (i + 3) % 7;
    v = i;
  }
  plot_raster(opened_raster);
  return 0;
}