The following example calculates each cell of a raster as the maximum of its four neighbours in the input raster.

```cpp
// example_5.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/offset_raster_view.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/transform_raster_view.h>

#include <algorithm>

int max_of_four(int a, int b, int c, int d)
{
  return std::max<int>(std::max<int>(a, b), std::max<int>(c, d));
}

namespace br = pronto::raster; 

int main()
{
  auto in = br::open<int>("demo.tif");// as produced in example_1

  int pad_value = 0;

  auto a = br::offset(in, 1, 0, pad_value); // row offset = 1, column offset = 0
  auto b = br::offset(in, -1, 0, pad_value);
  auto c = br::offset(in, 0, 1, pad_value);
  auto d = br::offset(in, 0, -1, pad_value);

  auto out = br::transform(max_of_four, a, b, c, d);

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