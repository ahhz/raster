// example_5.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/offset_raster_view.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/transform_raster_view.h>

#include <algorithm>

int max_of_four(int a, int b, int c, int d)
{
  return std::max<int>(std::max<int>(a, b), std::max<int>(c, d));
}

namespace pr = pronto::raster; 

int main()
{
  auto in = pr::open<int>("demo.tif");// as produced in example_1

  int pad_value = 0;

  auto a = pr::offset(in, 1, 0, pad_value); // row offset = 1, column offset = 0
  auto b = pr::offset(in, -1, 0, pad_value);
  auto c = pr::offset(in, 0, 1, pad_value);
  auto d = pr::offset(in, 0, -1, pad_value);

  auto out = pr::transform(max_of_four, a, b, c, d);

  plot_raster(in);
  plot_raster(out);

  return 0;
}
