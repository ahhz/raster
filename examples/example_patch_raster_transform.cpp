//example_patch_raster_transform.cpp

#include <blink/raster/patch_raster_transform.h>
#include <blink/raster/plot_raster.h>
#include <blink/raster/transform_raster_view.h>
#include <blink/raster/random_raster_view.h>

namespace br = blink::raster;

int main()
{
  // Choose a distribution to use, here the fair dice distribution
  auto dist = std::uniform_int_distribution<short>(1, 6);

  // The dimensions of the random raster
  auto rows = 6;
  auto cols = 5;

  // Create the random raster
  auto raster = br::random_distribution_raster(rows, cols, dist);

 
  auto patch = br::patch_raster(raster, br::queen_contiguity{});
  auto get_area = [](const br::patch_info& pi) {return pi.m_area; };
  auto patch_size = br::transform(br::optionalize_function(get_area), patch);

  plot_raster(raster);
  plot_raster(patch_size.sub_raster(1,1,2,2));
  plot_raster(patch_size);

  return 0;
}