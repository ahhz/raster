//example_optionalize.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/optional.h>
#include <pronto/raster/optional_raster_view.h>
#include <pronto/raster/padded_raster_view.h>
#include <pronto/raster/plot_raster.h>

namespace pr = pronto::raster;

int main()
{
  auto raster = pr::create_temp<int>(3, 4, GDT_Byte);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }
  auto optional_raster = pr::optionalize(raster);
  auto padded_optional_raster = pr::pad(optional_raster, 2, 2, 2, 2, pr::none);
  
  plot_raster(raster);
  plot_raster(optional_raster);
  plot_raster(padded_optional_raster);
  
  return 0;
}