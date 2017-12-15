//example_assign.cpp

#include <blink/raster/assign.h>
#include <blink/raster/io.h>
#include <blink/raster/plot_raster.h>

namespace br = blink::raster;

int main()
{
  auto a = br::create_temp<int>(3, 4, GDT_Byte);
  auto b = br::create_temp<int>(3, 4, GDT_Byte);
  int count = 0;
  for (auto&& i : a) {
    i = count++;
  }

  // copies the values in a over to b
  br::assign(b, a);

  for (auto&& i : a) {
    i = count++;
  }

  plot_raster(a);
  plot_raster(b);
}