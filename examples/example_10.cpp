//example_10.cpp
#include <blink/raster/plot_raster.h>
#include <blink/raster/random_raster_view.h>

#include <random> // for distributions and generators

namespace br = blink::raster;

int main()
{
  // Choose a distribution to use, here the fair dice distribution
  auto dist = std::uniform_int_distribution<short>(1, 6);
  
  // Choose a generator to use, and instantiate the generator with a seed
  auto gen = std::mt19937( std::random_device{}() );

  // The dimensions of the random raster
  auto rows = 16;
  auto cols = 8;

  // Create the random raster
  auto rr = br::random_distribution_raster(rows, cols, dist, gen);

  // Plot all values in the raster
  plot_raster(rr);

  // Or, plot only values of a subset of the raster.
  plot_raster(rr.sub_raster(1,1,4,4));

  return 0;
}