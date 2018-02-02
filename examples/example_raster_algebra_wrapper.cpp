//example_raster_algebra_wrapper.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/raster_algebra_operators.h>
#include <pronto/raster/raster_algebra_wrapper.h>

namespace pr = pronto::raster;

int main()
{
  auto raster =pr::create_temp<int>(3, 4);
  auto wrapped = pr::raster_algebra_wrap(raster);
  int i = 0;
  for (auto&& v : raster) {
    v = ((i+=3)%=7);
  }
  plot_raster(wrapped);
  plot_raster(wrapped + 1000);// using the raster algebra operator+()
  return 0;
}