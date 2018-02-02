//example_patch_raster_transform.cpp

#include <pronto/raster/patch_raster_transform.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/transform_raster_view.h>
#include <pronto/raster/random_raster_view.h>

namespace pr = pronto::raster;

int main()
{
  // Choose a distribution to use, here the fair dice distribution
  auto dist = std::uniform_int_distribution<short>(1, 6);

  // The dimensions of the random raster
  auto rows = 6;
  auto cols = 5;

  // Create the random raster
  auto raster = pr::random_distribution_raster(rows, cols, dist);

 
  auto patch = pr::patch_raster(raster, pr::queen_contiguity{});
  auto get_area = [](const pr::patch_info& pi) {return pi.m_area; };
  auto patch_size = pr::transform(pr::optionalize_function(get_area), patch);

  plot_raster(raster);
  plot_raster(patch_size.sub_raster(1,1,2,2));
  plot_raster(patch_size);

  return 0;
}