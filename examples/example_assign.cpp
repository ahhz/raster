//example_assign.cpp

#include <pronto/raster/assign.h>
#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>

namespace pr = pronto::raster;

int main()
{
  auto a = pr::create_temp<int>(3, 4, GDT_Byte);
  auto b = pr::create_temp<int>(3, 4, GDT_Byte);
  int count = 0;
  for (auto&& i : a) {
    i = count++;
  }

  // copies the values in a over to b
  pr::assign(b, a);

  for (auto&& i : a) {
    i = count++;
  }

  plot_raster(a);
  plot_raster(b);
}