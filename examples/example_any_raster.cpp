//example_any_raster.cpp

#include <blink/raster/any_raster.h>
#include <blink/raster/io.h>
#include <blink/raster/offset_raster_view.h>
#include <blink/raster/plot_raster.h>

namespace br = blink::raster;

// Here any_raster is used to implement a recursive function. Without 
// using the type erasure of any_raster it would be impossible to 
// determine the return type.

template<class T>
br::any_raster<T> recursive_function(br::any_raster<T> r)
{
  // continue offsetting by 1 cell until the corner cell is > 4 

  int pad_value = 999; // > 4 to guarantee that the recursion will end
  
  auto offset_view = br::offset(r, 1, 1, pad_value);
  if (*offset_view.begin() > 4) {
    return br::make_any_raster(offset_view);
  }
  else {
    return recursive_function(br::make_any_raster(offset_view));
  }
}

int main()
{
  auto raster = br::create_temp<int>(5, 4);

  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }

  auto result = recursive_function(br::make_any_raster(raster));

  plot_raster(raster);
  plot_raster(result);

  return 0;
}