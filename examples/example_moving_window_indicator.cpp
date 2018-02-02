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

namespace pr = pronto::raster;

int main()
{
  auto raster = pr::create_temp<int>( 7, 5);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 1) * 7 % 1000 ;
    v = i % 3;
  }

  // cell-based indicators
  auto window_most_common = pr::moving_window_indicator(
     raster
    , pr::circle(2) // alternatively pr::square(2)
    , pr::most_common_class_generator<int>{}
    );

  auto window_shannon_diversity = pr::moving_window_indicator(
    raster
    , pr::square(2) // alternatively pr::circle(2)
    , pr::shannon_diversity_generator{}
    );
    
  // edge-based indicators
  auto window_edge_density = pr::moving_window_indicator(
    raster
    , pr::edge_square(2)  // alternatively pr::edge_circle(2)
    , pr::edge_density_generator<int>{}
    );

  auto window_interspersion = pr::moving_window_indicator(
    raster
    , pr::edge_square(2)  // alternatively pr::edge_circle(2)
    , pr::interspersion_generator<int>{}
    );

  // patch-based indicators
  auto window_patch_size = pr::moving_window_indicator(
    raster
    , pr::patch_square(2) // alternatively pr::patch_circle(2)
    , pr::area_weighted_patch_size_generator{}
    , pr::rook_contiguity{} // alternatively pr::queen_contiguity{}
    );

  auto window_patch_shape = pr::moving_window_indicator(
    raster
    , pr::patch_circle(2) // alternatively pr::patch_square(2)
    , pr::patch_weighted_shape_index_generator{}
    , pr::queen_contiguity{} // alternatively pr::queen_contiguity{}
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