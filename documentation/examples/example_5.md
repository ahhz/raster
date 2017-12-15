The following example calculates each element of a raster as the maximum of its four neighbours in the input raster.

```cpp
// example_5.cpp
#include <blink/raster/raster.h>
#include <algorithm>

int max_of_four(int a, int b, int c, int d)
{
  return std::max<int>(std::max<int>(a, b), std::max<int>(c, d));
}

int main()
{
  namespace br = blink::raster;
  auto in = br::open<int>("demo.tif");

  int pad_value = 0;

  auto a = br::offset(in, 1, 0, pad_value); // row offset = 1, column offset = 0
  auto b = br::offset(in, -1, 0, pad_value);
  auto c = br::offset(in, 0, 1, pad_value);
  auto d = br::offset(in, 0, -1, pad_value);

  auto out = br::transform(max_of_four, a, b, c, d);

  plot_raster(out);

  return 0;
}
```