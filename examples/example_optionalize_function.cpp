//example_optionalize_function.cpp

#include <blink/raster/io.h>
#include <blink/raster/nodata_transform.h>
#include <blink/raster/optional.h>
#include <blink/raster/plot_raster.h>
#include <blink/raster/transform_raster_view.h>

namespace br = blink::raster;

int square(const int& v)
{
  return v * v;
}

int main()
{
  auto in = br::create_temp<int>(5, 4);
  int i = 0;
  for (auto&& v : in) {
    v = i;
    i = (i + 3) % 7;
  }

  // Treat value of 6 as nodata
  auto nodata_in = br::nodata_to_optional(in, 6);

  // Transform using square, skip over nodata values.
  auto nodata_in_sq = br::transform(br::optionalize_function(square), nodata_in);

  plot_raster(in);
  plot_raster(nodata_in);
  plot_raster(nodata_in_sq);

  return 0;
}