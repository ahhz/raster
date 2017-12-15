#include <blink/raster/raster.h>

int main()
{
  namespace br = blink::raster;

  auto in = br::open<int>("demo.tif");
  auto out = br::raster_algebra_wrap(in) * 5;

  br::plot_raster(out);

  return 0;
}