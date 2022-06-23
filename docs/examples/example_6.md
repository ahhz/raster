# Calculate statistics for a circular moving window
The following example creates a raster dataset and calculates the mean value of a surrounding square window for each cell. For larger rasters, the computational cost is proportional to the size of the raster, but independent of the radius (distance from the center cell to the side of the window).
```cpp
// example_6.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/moving_window_indicator.h>
#include <pronto/raster/plot_raster.h>

#include <pronto/raster/indicator/mean.h>

namespace pr = pronto::raster;

int main()
{
  auto in = pr::create_temp<int>(5, 6);
  for (int i = 0;  auto && v : in) {
    i = (i + 3) % 7;
    v = i;
  }

  auto window = pr::square(2);
  auto indicator = pr::mean_generator<int>{};

  auto out = pr::moving_window_indicator(in, window, indicator);

  plot_raster(in);
  plot_raster(out);

  return 0;
}
}
```
Output:
```
Rows: 5, Cols: 6, Value type: int
3       6       2       5       1       4
0       3       6       2       5       1
4       0       3       6       2       5
1       4       0       3       6       2
5       1       4       0       3       6

Rows: 5, Cols: 6, Value type: class std::optional<double>
3       3.33333 3.2     3.4     3.5     3.44444
2.66667 3       3.1     3.3     3.3125  3.5
2.8     2.9     3       3.2     3.3     3.4
2.58333 2.625   2.9     3.1     3.375   3.41667
2.44444 2.58333 2.8     3       3.33333 3.66667
```