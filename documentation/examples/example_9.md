The following example shows how the library can be used to iterator over subrasters that are windows centered on each cell. Note that the subrasters are not formed by copying over the data, but do in fact refer to the same data.

```cpp
//example_9.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/subraster_window_view.h>

namespace pr = pronto::raster;

int main()
{
  auto in = pr::open<int>("demo.tif");
  
  int radius = 1;
  int pad_value = -99;

  auto window = pr::make_square_subraster_window_view(in, radius, pad_value);

  plot_raster(in);

  // to avoid plotting too much, only plot only the windows for the cells 
  // in the first two rows and three columns.
  for (auto&& i : window.sub_raster(0, 0, 2, 3) ) {
    plot_raster(i);
  }
  
  return 0;
}
```
Output: 
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

Rows: 3, Cols: 3, Value type: int
-99     -99     -99
-99     3       1
-99     2       6

Rows: 3, Cols: 3, Value type: int
-99     -99     -99
3       1       3
2       6       6

Rows: 3, Cols: 3, Value type: int
-99     -99     -99
1       3       6
6       6       1

Rows: 3, Cols: 3, Value type: int
-99     3       1
-99     2       6
-99     2       6

Rows: 3, Cols: 3, Value type: int
3       1       3
2       6       6
2       6       1

Rows: 3, Cols: 3, Value type: int
1       3       6
6       6       1
6       1       1
```
