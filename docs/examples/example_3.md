# Sum two rasters cell-by-cell
The following example is a simple case of opening two rasters and performing an cell-by-cell summation.
```cpp
// example_3.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/raster_algebra_operators.h>
#include <pronto/raster/raster_algebra_wrapper.h>

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

  plot_raster(c);

  return 0;
}
```
Output:
```
Rows: 4, Cols: 5, Value type: int
101     202     303     404     505
606     707     808     909     1010
1111    1212    1313    1414    1515
1616    1717    1818    1919    2020
```