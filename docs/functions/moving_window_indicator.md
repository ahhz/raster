# moving_window_indicator

## Prototype
```cpp
template<class Raster, class IndicatorGenerator>
auto moving_window_indicator(Raster raster, const circle& window
, IndicatorGenerator indicator_generator);

template<class Raster, class IndicatorGenerator>
auto moving_window_indicator(Raster raster, const square& window
, IndicatorGenerator indicator_generator);

template<class Raster, class IndicatorGenerator>
auto moving_window_indicator(Raster raster, const square_edge& window
, IndicatorGenerator indicator_generator);
```

## Description
Creates an alternate view of the input raster Raster that for each element returns a statistic (indicator) calculated for the window surrounding it. Three different types of window are supported `square`, `circle` and `square_edge`.

These are simple structs that are constructed using a single argument (radius).

## Definition
<pronto/raster/moving_window_indicator.h> [(open in Github)](https://github.com/ahhz/raster/blob/master/include/pronto/raster/moving_window_indicator.h)

## Requirements on types
Raster must implement the RasterView concept. IndicatorGenerator must implement the IndicatorGenerator concept.

## Preconditions
`raster` must be initialized

## Complexity
The complexity of this function is O(1), but the complexity of iterating over the moving_window_view depends on the window type used. For square windows it is O(n) and for circular windows it is O(n*r) where n is the size of the raster and r is the radius of the window. 

## Example of use

```
//example_moving_window_indicator.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/moving_window_indicator.h>
#include <pronto/raster/plot_raster.h>

#include <pronto/raster/indicator/mean.h>

namespace pr = pronto::raster;

int main()
{
  auto raster = pr::create_temp<int>( 3, 4, GDT_Byte);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }
  
  auto window_mean = pr::moving_window_indicator(raster, pr::square(1), 
    pr::mean_generator<int>{});

  plot_raster(raster);
  plot_raster(window_mean);
  plot_raster(window_mean.sub_raster(1,1,2,2));

  return 0;
}
```
Output:
```
Rows: 3, Cols: 4, Value type: int
3       6       2       5
1       4       0       3
6       2       5       1

Rows: 3, Cols: 4, Value type: class std::optional<double>
3.5     2.66667 3.33333 2.5
3.66667 3.22222 3.11111 2.66667
3.25    3       2.5     2.25

Rows: 2, Cols: 2, Value type: class std::optional<double>
3.22222 3.11111
3       2.5

```
## Notes

## See also


