# pad
## Prototype
```cpp
template<Raster> void plot_raster(Raster raster);
void plot_raster(any_blind_raster raster);
```
  
## Description
Takes a raster and sends the contents to std::cout. 

## Definition
[<blink/raster/plot_raster.h>](./../../include/blink/raster/plot_raster.h)

## Requirements on types
Raster must implement the RasterView concept

## Preconditions
When the raster has more than 1000 cells, the individual values are not plotted.

## Complexity
O(n), where n is the size of the raster.

## Example of use
```cpp
// example_1.cpp
#include <blink/raster/io.h>
#include <blink/raster/plot_raster.h>

#include <random>

namespace br = blink::raster;

int main()
{
  std::default_random_engine rng;
  std::uniform_int_distribution<int> dice(1, 6);

  auto raster = br::create<int>("demo.tif", 10, 5);
  for (auto&& v : raster) {
    v = dice(rng);
  }
  plot_raster(raster);
  return 0;
}
```

Output (will vary due to random number generator):
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
```
## Notes
This function is intended for use on very small rasters for debugging and exposition purposes. 

## See also

