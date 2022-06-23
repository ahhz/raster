# Multiply raster by constant
The following example is a simple case of opening a raster and performing an cell-by-cell multiplication.

```cpp
//example_4.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/raster_algebra_operators.h>

namespace pr = pronto::raster;

void create_raster_for_demo()
{
  auto a = pr::create<int>("a.tif", 4, 5);

  for (int i = 0; auto && v : a) {
    v = ++i;
  }
}

int main()
{
  create_raster_for_demo();

  auto in = pr::open_variant("a.tif"); // assuming raster exists
  auto out = in * 5;

  plot_raster(in);
  plot_raster(out);

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
5       10      15      20      25
30      35      40      45      50
55      60      65      70      75
80      85      90      95      100
```

