// example_6.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/moving_window_indicator.h>
#include <pronto/raster/plot_raster.h>

#include <pronto/raster/indicator/mean.h>

namespace br = pronto::raster;

int main()
{
  auto in = br::create_temp<int>(5, 6);
  int i = 0;
  for (auto&& v : in) {
    i = (i + 3) % 7;
    v = i;
  }

  auto window = br::square(2);
  auto indicator = br::mean_generator<int>{};

  auto out = br::moving_window_indicator(in, window, indicator);

  plot_raster(in);
  plot_raster(out);

  return 0;
}