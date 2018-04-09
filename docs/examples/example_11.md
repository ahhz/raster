# Apply a function to a subsection of raster
Examples [9](./example_9.md) and [10](./example_10.md) already included the use of the `sub_raster` member function. This is an fundamental function in the library, because it can be used with all supported rasters. Where possible functions on rasters are lazy and don't evaluate cell-values until they are needed. Functions such as offset, transform, and raster algebra operations do not actually iterate over values, but produce a expression template that is itself a raster of which a sub_raster can be taken. 

Thus, in the example below, the + operation is only applied on six cells.
 
```cpp
// example_11.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/raster_algebra.h>
#include <pronto/raster/raster_algebra_operators.h>

namespace pr = pronto::raster;

void create_rasters_for_demo()
{
  auto a = pr::create<int>("a.tif", 4, 5);
  auto b = pr::create<int>("b.tif", 4, 5);
  int i = 0; for (auto&& v : a) { v = ++i; }
  int j = 0; for (auto&& w : b) { j += 100;  w = j; }
}

int main()
{
  create_rasters_for_demo();

  auto a = pr::open<int>("a.tif"); // assuming raster exists
  auto b = pr::open<int>("b.tif"); // assuming raster exists and same dimensions as "a.tif"
  auto c = pr::raster_algebra_wrap(a) + pr::raster_algebra_wrap(b);

  
  plot_raster(a);
  plot_raster(b);

  int start_row = 1;
  int start_col = 2;
  int rows = 2;
  int cols = 3;

  plot_raster(c.sub_raster(start_row, start_col, rows, cols) );

  return 0;
}
```

Output: 
```
Rows: 4, Cols: 5, Value type: int
1       2       3       4       5
6       7       8       9       10
11      12      13      14      15
16      17      18      19      20

Rows: 4, Cols: 5, Value type: int
100     200     300     400     500
600     700     800     900     1000
1100    1200    1300    1400    1500
1600    1700    1800    1900    2000

Rows: 2, Cols: 3, Value type: int
808     909     1010
1313    1414    1515
```
