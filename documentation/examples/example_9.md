The following example shows how the library can be used to iterator over subrasters that are windows centered on each pixel.
```cpp
//example_9.cpp
#include <blink/raster/raster.h>
#include <blink/raster/subraster_window_view.h>

namespace br = blink::raster;

int main()
{
  auto in = br::open<int>("demo.tif");
  
  int radius = 2;
  int pad_value = -99;

  auto window = br::make_square_subraster_window_view(in, radius, pad_value);
  
  // to avoid plotting to much only plot first two rows and three columns.
  for (auto&& i : window.sub_raster(0,0,2,3) ) 
  {
    plot_raster(i);
  }
  
  return 0;
}
```