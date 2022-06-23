//example_4.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/raster_algebra_operators.h>

namespace pr = pronto::raster;

void create_raster_for_demo()
{
  auto a = pr::create<int>("a.tif", 4, 5);

  for (int i = 0; auto && v : a) {
    v = ++i;
  }
}

int main()
{
  create_raster_for_demo();

  auto in = pr::open_variant("a.tif"); // assuming raster exists
  auto out = in * 5;

  plot_raster(in);
  plot_raster(out);

  return 0;
}
