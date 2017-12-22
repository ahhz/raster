# raster_algebra_wrap
## Prototype
```cpp
template<class Raster>
raster_algebra_wrapper<Raster> raster_algebra_wrap(Raster raster);
```
## Description
Wraps a raster in a raster_algebra_wrapper, so it can be used in raster algebra operations. 

## Definition
[<blink/raster/raster_algebra_wrapper.h>](./../../include/blink/raster/raster_algebra_wrapper.h)

## Requirements on types
Raster must implement the RasterView concept.

## Preconditions
`raster` must be initialized

## Complexity
O(1)

## Example of use
```cpp
//example_raster_algebra_wrapper.cpp

#include <blink/raster/io.h>
#include <blink/raster/plot_raster.h>
#include <blink/raster/raster_algebra_operators.h>
#include <blink/raster/raster_algebra_wrapper.h>

namespace br = blink::raster;

int main()
{
  auto raster =br::create_temp<int>(3, 4);
  auto wrapped = br::raster_algebra_wrap(raster);
  int i = 0;
  for (auto&& v : raster) {
    v = ((i+=3)%=7);
  }
  plot_raster(wrapped);
  plot_raster(wrapped + 1000);// using the raster algebra operator+()
  return 0;
}
```
Output:
```
Rows: 3, Cols: 4, Value type: int
3       6       2       5
1       4       0       3
6       2       5       1

Rows: 3, Cols: 4, Value type: int
1003    1006    1002    1005
1001    1004    1000    1003
1006    1002    1005    1001
```
## Notes

## See also

