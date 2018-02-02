// example_3.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/raster_algebra_operators.h>
#include <pronto/raster/raster_algebra_wrapper.h>

namespace br = pronto::raster;

void create_rasters_for_demo()
{
  auto a = br::create<int>("a.tif", 4, 5);
  auto b = br::create<int>("b.tif", 4, 5);
  int i = 0; for (auto&& v : a) { v = ++i; }
  int j = 0; for (auto&& w : b) { j += 100;  w = j; }
}

int main()
{
  create_rasters_for_demo();
  auto a = br::open<int>("a.tif"); // assuming raster exists
  auto b = br::open<int>("b.tif"); // assuming raster exists and same dimensions as "a.tif"
  auto c = br::raster_algebra_wrap(a) + br::raster_algebra_wrap(b);

  plot_raster(c);

  return 0;
}