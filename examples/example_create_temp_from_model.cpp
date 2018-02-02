//example_create_temp_from_model.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>

namespace pr = pronto::raster;

int main()
{
  auto model = pr::create<int>("test.tif", 3, 4);

  auto output = pr::create_temp_from_model<float>(model);
  float i = 0.7f;
  for (auto&& v : output) {
    i = i * (1 - i);
    v = i;
  }
  plot_raster(output);
  return 0;
}