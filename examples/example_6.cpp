// example_6.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/moving_window_indicator.h>
#include <pronto/raster/plot_raster.h>

#include <pronto/raster/indicator/mean.h>

namespace pr = pronto::raster;

int main()
{
  auto in = pr::create_temp<int>(5, 6);
  for (int i = 0;  auto && v : in) {
    i = (i + 3) % 7;
    v = i;
  }

  auto window = pr::square(2);
  auto indicator = pr::mean_generator<int>{};

  auto out = pr::moving_window_indicator(in, window, indicator);

  plot_raster(in);
  plot_raster(out);

  return 0;
}