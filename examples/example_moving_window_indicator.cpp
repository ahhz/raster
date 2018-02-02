//example_moving_window_indicator.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/moving_window_indicator.h>
#include <pronto/raster/optional.h>
#include <pronto/raster/plot_raster.h>

#include <pronto/raster/indicator/area_weighted_patch_size.h>
#include <pronto/raster/indicator/count.h>
#include <pronto/raster/indicator/edge_density.h>
#include <pronto/raster/indicator/interspersion.h>
#include <pronto/raster/indicator/mean.h>
#include <pronto/raster/indicator/most_common_class.h>
#include <pronto/raster/indicator/patch_weighted_shape_index.h>
#include <pronto/raster/indicator/shannon_diversity.h>

namespace br = pronto::raster;

int main()
{
  auto raster = br::create_temp<int>( 7, 5);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 1) * 7 % 1000 ;
    v = i % 3;
  }

  // cell-based indicators
  auto window_most_common = br::moving_window_indicator(
     raster
    , br::circle(2) // alternatively br::square(2)
    , br::most_common_class_generator<int>{}
    );

  auto window_shannon_diversity = br::moving_window_indicator(
    raster
    , br::square(2) // alternatively br::circle(2)
    , br::shannon_diversity_generator{}
    );
    
  // edge-based indicators
  auto window_edge_density = br::moving_window_indicator(
    raster
    , br::edge_square(2)  // alternatively br::edge_circle(2)
    , br::edge_density_generator<int>{}
    );

  auto window_interspersion = br::moving_window_indicator(
    raster
    , br::edge_square(2)  // alternatively br::edge_circle(2)
    , br::interspersion_generator<int>{}
    );

  // patch-based indicators
  auto window_patch_size = br::moving_window_indicator(
    raster
    , br::patch_square(2) // alternatively br::patch_circle(2)
    , br::area_weighted_patch_size_generator{}
    , br::rook_contiguity{} // alternatively br::queen_contiguity{}
    );

  auto window_patch_shape = br::moving_window_indicator(
    raster
    , br::patch_circle(2) // alternatively br::patch_square(2)
    , br::patch_weighted_shape_index_generator{}
    , br::queen_contiguity{} // alternatively br::queen_contiguity{}
    );


  plot_raster(raster);
  
  plot_raster(window_most_common);
  plot_raster(window_shannon_diversity);

  plot_raster(window_edge_density);
  plot_raster(window_interspersion);

  plot_raster(window_patch_size);
  plot_raster(window_patch_shape);

 

  return 0;
}