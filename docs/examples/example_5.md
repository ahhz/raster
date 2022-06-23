# Apply custom function on a four-cell moving window
The following example calculates each cell of a raster as the maximum of its four neighbours in the input raster.

```cpp
// example_5.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/offset_raster_view.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/transform_raster_view.h>

#include <algorithm>
#include <random>

namespace pr = pronto::raster;

auto create_temporary_raster_for_demo()
{
  // See also example_1
  auto rng = std::default_random_engine{};
  auto dice = std::uniform_int_distribution<int>{ 1, 6 };

  auto raster = pr::create_temp<int>(10, 5);
  for (auto&& v : raster) {
    v = dice(rng);
  }
  return raster;
}

int max_of_four(int a, int b, int c, int d)
{
  return std::max({a,b,c,d});
}

namespace pr = pronto::raster; 

int main()
{
  auto in = create_temporary_raster_for_demo();
 
  int pad_value = 0;

  auto a = pr::offset(in,  1,  0, pad_value); // row offset = 1, column offset = 0
  auto b = pr::offset(in, -1,  0, pad_value);
  auto c = pr::offset(in,  0,  1, pad_value);
  auto d = pr::offset(in,  0, -1, pad_value);

  auto out = pr::transform(max_of_four, a, b, c, d);

  plot_raster(in);
  plot_raster(out);

  return 0;
}
```
Ouput: 
```
Rows: 10, Cols: 5, Value type: int
3       1       3       6       5
2       6       6       1       2
2       6       1       1       6
1       4       5       2       5
6       2       6       2       1
3       5       4       5       6
1       4       2       3       1
2       2       6       2       1
1       1       6       1       6
1       4       5       2       3

Rows: 10, Cols: 5, Value type: int
2       6       6       5       6
6       6       6       6       6
6       6       6       6       5
6       6       6       5       6
3       6       5       6       6
6       4       6       6       5
4       5       6       5       6
2       6       6       6       6
2       6       6       6       3
4       5       6       5       6
```