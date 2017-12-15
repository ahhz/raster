#include <blink/raster/io.h>
#include <blink/raster/plot_raster.h>

#include <random>

int main()
{
  namespace br = blink::raster;

  std::default_random_engine rng;
  std::uniform_int_distribution<int> dice(1, 6);

  auto raster = br::create<int>("demo.tif", 10, 5);
  for (auto&& v : raster) {
    v = dice(rng);
  }

  plot_raster(raster);

  return 0;
}