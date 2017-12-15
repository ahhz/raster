[Example 1](.\example_1.md) showed how a raster can be filled with random values following a probability distribution. That solution however is not useful when only a subset of random values in a larger raster of random values is needed. Therefore the library offers its own random_raster_view function to create a raster of random values of which subsets can be accessed without precomputing all random values in the raster. 

 
```cpp
//example_10.cpp
#include <blink/raster/raster.h>
#include <blink/raster/random_raster_view.h>

#include <random> // for distributions and generators

namespace br = blink::raster;

int main()
{
  // Choose a distribution to use, here the fair dice distribution
  auto dist = std::uniform_int_distribution<short>(1, 6);
  
  // Choose a generator to use, and instantiate the generator with a seed
  auto gen = std::mt19937( std::random_device{}() );

  // The dimensions of the random raster
  auto rows = 16;
  auto cols = 8;

  // Create the random raster
  auto rr = br::random_distribution_raster(rows, cols, dist, gen);

  // Plot all values in the raster
  plot_raster(rr);

  // Or, plot only values of a subset of the raster.
  plot_raster(rr.sub_raster(1,1,4,4));

  return 0;
}
```

Output: (the exact output will vary due to the use of the random_device)

```
Rows: 16, Cols: 8, Value type: short
4       5       2       4       6       1       4       3
1       6       3       5       4       6       5       1
6       1       1       6       2       6       5       1
2       2       4       5       3       6       6       4
2       5       6       6       6       4       4       4
6       3       1       2       6       4       5       3
2       4       4       3       2       6       3       1
1       6       6       5       2       5       6       2
3       2       4       2       2       6       5       3
2       6       6       5       2       2       4       6
1       2       5       5       6       5       3       4
6       2       1       4       5       3       4       3
1       2       4       3       5       3       2       3
6       5       3       6       5       3       6       4
4       3       1       6       6       4       6       6
5       5       4       3       3       2       1       3

Rows: 4, Cols: 4, Value type: short
6       3       5       4
1       1       6       2
2       4       5       3
5       6       6       6
```
