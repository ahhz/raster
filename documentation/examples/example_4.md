The following example is a simple case of opening a raster and performing an element-by-element multiplication.

```cpp
// example_4.cpp
#include <blink/raster/raster.h>

int main()
{
  namespace br = blink::raster;

  auto in = br::open<int>("demo.tif");
  auto out = br::raster_algebra_wrap(in) * 5;

  br::plot_raster(out);
  return 0;
}
```