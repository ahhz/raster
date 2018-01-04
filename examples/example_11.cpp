// example_11.cpp

#include <blink/raster/io.h>
#include <blink/raster/plot_raster.h>
#include <blink/raster/raster_algebra_operators.h>
#include <blink/raster/raster_algebra_wrapper.h>

namespace br = blink::raster;

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

  
  plot_raster(a);
  plot_raster(b);

  int start_row = 1;
  int start_col = 2;
  int rows = 2;
  int cols = 3;

  plot_raster(c.sub_raster(start_row, start_col, rows, cols) );

  return 0;
}