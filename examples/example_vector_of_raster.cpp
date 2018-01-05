//example_vector_of_raster.cpp

#include <blink/raster/io.h>
#include <blink/raster/plot_raster.h>
#include <blink/raster/vector_of_raster_view.h>

namespace br = blink::raster;

struct get_int_string
{
  template<class T> 
  int operator()(const std::vector<T>& v) const
  {
    int out = 0;
    for (auto&& i : v) {
      out = out * 10 + i;
    }
    return out;
  }
};


int main()
{
  std::vector<br::gdal_raster_view<int>> rasters;

  int i = 0;
  for (int j = 0; j < 5; ++j)
  {
    auto raster = br::create_temp<int>(3, 4, GDT_Byte);

    for (auto&& v : raster) {
      i = (i + 3) % 7;
      v = i;
    }
    rasters.emplace_back(raster);
  }
  auto vr = br::raster_vector(rasters);
  auto str_vr = br::transform(get_int_string{}, vr);
  plot_raster(str_vr);
  return 0;
}