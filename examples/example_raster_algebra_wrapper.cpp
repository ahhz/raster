//example_raster_algebra_wrapper.cpp

#include <blink/raster/io.h>
#include <blink/raster/plot_raster.h>
#include <blink/raster/raster_algebra_operators.h>
#include <blink/raster/raster_algebra_wrapper.h>

namespace br = blink::raster;

int main()
{
  auto raster =br::create_temp<int>(3, 4);
  auto wrapped = br::raster_algebra_wrap(raster);
  int i = 0;
  for (auto&& v : raster) {
    v = ((i+=3)%=7);
  }
  plot_raster(wrapped);
  plot_raster(wrapped + 1000);// using the raster algebra operator+()
  return 0;
}