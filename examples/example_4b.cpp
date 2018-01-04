//example_4b.cpp

#include <blink/raster/io.h>
#include <blink/raster/plot_raster.h>
#include <blink/raster/raster_algebra_operators.h>
#include <blink/raster/raster_algebra_wrapper.h>

namespace br = blink::raster;

int main()
{
  auto in = br::open_any("a.tif"); // as created in example_3.cpp
  auto out = br::raster_algebra_wrap(in) * 5;
 
  plot_raster(in);
  plot_raster(out);

  return 0;
}