//example_optionalize_function.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/nodata_transform.h>
#include <pronto/raster/optional.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/transform_raster_view.h>

namespace pr = pronto::raster;

int square(const int& v)
{
  return v * v;
}

int main()
{
  auto in = pr::create_temp<int>(5, 4);
  int i = 0;
  for (auto&& v : in) {
    v = i;
    i = (i + 3) % 7;
  }

  // Treat value of 6 as nodata
  auto nodata_in = pr::nodata_to_optional(in, 6);

  // Transform using square, skip over nodata values.
  auto nodata_in_sq = pr::transform(pr::optionalize_function(square), nodata_in);

  plot_raster(in);
  plot_raster(nodata_in);
  plot_raster(nodata_in_sq);

  return 0;
}