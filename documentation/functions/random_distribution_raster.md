# make_any_raster
## Prototype
```cpp
template<class Distribution, class Generator = std::mt19937_64
      , int BlockRows = 512, int BlockCols = 512>
    cached_random_blocks<Distribution, Generator, BlockRows, BlockCols>
      random_distribution_raster(int rows, int cols
        , Distribution dist
        , Generator gen = Generator(std::random_device()));
```
## Description
Create a raster of random distributed values using a specific random number generator (by default std::mt19937_64), and using buffered values (by default in blocks of 512 rows by 512 columns). The return type is a model of RasterView.

## Definition
[<blink/raster/random_distribution_raster.h>](./../../include/blink/raster/random_distribution_raster.h)

## Requirements on types
`Distribution` must be a model of [RandomNumberDistribution](http://en.cppreference.com/w/cpp/concept/RandomNumberDistribution)
`Generator` must be a model of [UniformRandomBitGenerator](http://en.cppreference.com/w/cpp/concept/UniformRandomBitGenerator)


## Preconditions
`rows` and `cols` must be positive.

## Complexity
The raster of random values makes use of a buffer of blockwise precomputed random values. In the construction a random seed is calculated for each block and hence the complexity is O(n) where n is the number of blocks (and hence logically it is also O(m) where m is the number of pixels). However there are many fewer blocks than pixels (by default by a factor of 512 x 512 = 262144), so in many situations this cost of construction can be neglected.

## Example of use

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

## Notes
The random values raster view uses a blockwise buffer and a limited number of blocks is kept in memory using a last-used-first-out mechanism. Each block has a fixed random seed, so upon reloading the same numbers are regenerated.

## See also