#include <blink/raster/raster.h>

int main()
{
  namespace br = blink::raster;

  auto a = br::open<int>("a.tif"); // assuming raster exists
  auto b = br::open<int>("b.tif"); // assuming raster exists and same dimensions as "a.tif"
  auto c = br::raster_algebra_wrap(a) + br::raster_algebra_wrap(b);

  plot_raster(c);

  return 0;
}