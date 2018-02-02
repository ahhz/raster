// example_11.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/raster_algebra_operators.h>
#include <pronto/raster/raster_algebra_wrapper.h>

namespace pr = pronto::raster;

void create_rasters_for_demo()
{
  auto a = pr::create<int>("a.tif", 4, 5);
  auto b = pr::create<int>("b.tif", 4, 5);
  int i = 0; for (auto&& v : a) { v = ++i; }
  int j = 0; for (auto&& w : b) { j += 100;  w = j; }
}

int main()
{
  create_rasters_for_demo();

  auto a = pr::open<int>("a.tif"); // assuming raster exists
  auto b = pr::open<int>("b.tif"); // assuming raster exists and same dimensions as "a.tif"
  auto c = pr::raster_algebra_wrap(a) + pr::raster_algebra_wrap(b);

  
  plot_raster(a);
  plot_raster(b);

  int start_row = 1;
  int start_col = 2;
  int rows = 2;
  int cols = 3;

  plot_raster(c.sub_raster(start_row, start_col, rows, cols) );

  return 0;
}