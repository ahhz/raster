The following example is a simple case of opening a raster and performing an cell-by-cell multiplication.

```cpp
//example_4.cpp

#include <blink/raster/io.h>
#include <blink/raster/plot_raster.h>
#include <blink/raster/raster_algebra_operators.h>
#include <blink/raster/raster_algebra_wrapper.h>

namespace br = blink::raster;

int main()
{
  auto in = br::open<int>("a.tif"); // as created in example_3.cpp
  auto out = br::raster_algebra_wrap(in) * 5;
 
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
Note that it is not required to specify the value type of the cells, using `open_any` instead of `open`. However, this is at some cost of performance.

```cpp
//example_4b.cpp

#include <blink/raster/io.h>
#include <blink/raster/plot_raster.h>
#include <blink/raster/raster_algebra.h>
#include <blink/raster/raster_algebra_operators.h>

namespace br = blink::raster;

int main()
{
  auto in = br::open_any("a.tif"); // as created in example_3.cpp
  auto out = br::raster_algebra_wrap(in) * 5;
 
  plot_raster(in);
  plot_raster(out);

  return 0;
}
```
