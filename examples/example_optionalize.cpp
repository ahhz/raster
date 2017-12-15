//example_optionalize.cpp

#include <blink/raster/io.h>
#include <blink/raster/optional.h>
#include <blink/raster/optional_raster_view.h>
#include <blink/raster/padded_raster_view.h>
#include <blink/raster/plot_raster.h>

namespace br = blink::raster;

int main()
{
  auto raster = br::create_temp<int>(3, 4, GDT_Byte);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }
  auto optional_raster = br::optionalize(raster);
  auto padded_optional_raster = br::pad(optional_raster, 2, 2, 2, 2, br::none);
  
  plot_raster(raster);
  plot_raster(optional_raster);
  plot_raster(padded_optional_raster);
  
  return 0;
}