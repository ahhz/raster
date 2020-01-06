//example_of_reinterpret_raster.cpp
// this function is not documented yet, it was developed with the purpose of 
// having rasters with large int values.
//
// Here two int raster are reinterpreted as a single long long raster.
//
// This solution may also be useful for rasters of other unusual value types,
// e.g. structs and pointers.

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/reinterpret_raster_view.h>

namespace pr = pronto::raster;

int main()
{
  auto a = pr::create_temp<std::int32_t>(5, 5);
  auto b = pr::create_temp<std::int32_t>(5, 5);
  auto int64_raster = pr::reinterpret_rasters<std::int64_t> (a, b);
  
  int i = 1;
  
  for (auto&& v : int64_raster) {
      i = (i + 3) % 7;
      v =  (i+1) * 100000001000LL;
  }
  plot_raster(int64_raster);
  return 0;
}