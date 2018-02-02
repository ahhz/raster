//example_4.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/raster_algebra_operators.h>
#include <pronto/raster/raster_algebra_wrapper.h>

namespace br = pronto::raster;

int main()
{
  auto in = br::open<int>("a.tif"); // as created in example_3.cpp
  auto out = br::raster_algebra_wrap(in) * 5;
 
  plot_raster(in);
  plot_raster(out);

  return 0;
}