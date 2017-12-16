// example_7.cpp

#include <blink/raster/io.h>
#include <blink/raster/moving_window_indicator.h>
#include <blink/raster/plot_raster.h>

#include <blink/raster/indicator/mean.h>

namespace br = blink::raster;

int main()
{
  auto in = br::create_temp<int>(5, 6);
  int i = 0;
  for (auto&& v : in) {
    i = (i + 3) % 7;
    v = i;
  }

  auto window = br::circle(2);
  auto indicator = br::mean_generator<int>{};

  auto out = br::moving_window_indicator(in, window, indicator);

  plot_raster(in);
  plot_raster(out);

  return 0;
}