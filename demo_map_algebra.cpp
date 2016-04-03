// This demo combines the blink raster, iterator and moving window libraries for map_algebra type operations

#include <blink/raster/utility.h> // To open rasters
#include <blink/iterator/range_algebra.h> // To apply "map algebra" type operations on rasters
#include <blink/moving_window/moving_window_view.h> // To make moving window views of rasters
#include <blink/moving_window/indicator/edge_density.h> // The moving window indicator used as an example

using blink::raster::open_gdal_raster; 
using blink::raster::create_gdal_raster;
using blink::iterator::range_algebra;
using blink::raster::create_gdal_raster_from_model;
using blink::iterator::make_range_algebra_transform;

int my_function(int w, int x, int y, int z)
{
  return w * x + y * z;
}

int main()
{

  auto a = open_gdal_raster<int>("input_1.tif", GA_ReadOnly);
  auto b = open_gdal_raster<int>("input_2.tif", GA_ReadOnly);

  // Example 1: Plain map algebra using operators
  auto output_1 = create_gdal_raster_from_model<double>("output_1.tif", a);
  output_1 = range_algebra(a) + 3 * range_algebra(b);
 
  // Example 2: Map algebra using cell-by-cell functions
  auto output_2 = create_gdal_raster_from_model<double>("output_2.tif", a);
  output_2 = make_range_algebra_transform(my_function, 1, range_algebra(a), 3, range_algebra(b));

  // Example 3: Combination
  auto output_3 = create_gdal_raster_from_model<double>("output_3.tif", a);
  output_3 = range_algebra(a) + 3 * range_algebra(make_range_algebra_transform(my_function, 1, range_algebra(a), 3, range_algebra(b)));

  // Example 4: Moving window

  return 0;
}