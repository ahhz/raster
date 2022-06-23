// example_1.cpp
#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>

#include <random>

namespace pr = pronto::raster;

int main()
{
  auto rng = std::default_random_engine{};
  auto dice = std::uniform_int_distribution<int>{ 1, 6 };

  auto raster = pr::create<int>("demo.tif", 10, 5);
  for (auto&& v : raster) {
    v = dice(rng);
  }
  plot_raster(raster);
  return 0;
}