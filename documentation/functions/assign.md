# assign 
## Prototype
```cpp
template<class RasterTo, class RasterFrom>
void assign(RasterTo& to, const RasterFrom& from);
```

## Description
Assigns the value of each element in `Raster` `from` to the corresponding element in `Raster` `to`
## Definition
[<pronto/raster/assign.h>](./../../include/pronto/raster/assign.h)

## Requirements on types
The value_type of RasterFrom must be castable to the value_type of RasterTo. RasterTo must be mutable. RasterFrom and RasterTo both must be Rasters.

## Preconditions
`to` and `from` must have the same dimensions

## Complexity
The complexity is the cost of iterating over all elements in `to` and `from`. It is at least O(n) where n is the number of cells but can be more depending on the type of `from` and `to`. 

## Example of use
```cpp
//example_assign.cpp

#include <pronto/raster/assign.h>
#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>

namespace br = pronto::raster;

int main()
{
  auto a = br::create_temp<int>(3, 4, GDT_Byte);
  auto b = br::create_temp<int>(3, 4, GDT_Byte);
  int count = 0;
  for (auto&& i : a) {
    i = count++;
  }

  // copies the values in a over to b
  br::assign(b, a);

  for (auto&& i : a) {
    i = count++;
  }

  plot_raster(a);
  plot_raster(b);
}
```
Output:
```
Rows: 3, Cols: 4, Value type: int
12      13      14      15
16      17      18      19
20      21      22      23

Rows: 3, Cols: 4, Value type: int
0       1       2       3
4       5       6       7
8       9       10      11
```

## Notes

## See also

