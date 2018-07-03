#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include <gtest/gtest.h>

#include <pronto/raster/random_raster_view.h>
#include <pronto/raster/plot_raster.h>
#include <random>
#include <vector>

namespace pr = pronto::raster;

bool create_random_raster_view()
{
  auto r = pr::random_distribution_raster(5, 5, std::uniform_int_distribution<>(1, 6), std::mt19937());
  pr::plot_raster(r);
  for (auto&& i : std::as_const(r))
  {
    if (i < 1 || i > 6) return false;
  }
  return true;
}


TEST(RasterTest, RandomRasterView) {
	EXPECT_TRUE(create_random_raster_view());
  
}