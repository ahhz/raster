# Calculate statistics for a square moving window
The following example opens a raster dataset and calculates the mean value of a surrounding circular window for each cell. For larger maps, the computational cost is proportional to the size of the raster multiplied by the radius.
```cpp
// example_7.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/moving_window_indicator.h>
#include <pronto/raster/plot_raster.h>

#include <pronto/raster/indicator/mean.h>

namespace pr = pronto::raster;

int main()
{
  auto in = pr::create_temp<int>(5, 6);
  int i = 0;
  for (auto&& v : in) {
    i = (i + 3) % 7;
    v = i;
  }

  auto window = pr::circle(2);
  auto indicator = pr::mean_generator<int>{};

  auto out = pr::moving_window_indicator(in, window, indicator);

  plot_raster(in);
  plot_raster(out);

  return 0;
}
```
Output:
```
Rows: 5, Cols: 6, Value type: class std::optional<double>
3       3.125   3.44444 4.11111 2.75    3.5
2.875   3       3.16667 3.25    3.90909 2.75
2.55556 2.83333 3       3.30769 3.25    4.11111
1.875   2.54545 2.83333 2.91667 3.45455 3.5
3.16667 1.875   2.55556 3.22222 3.25    3.66667
```