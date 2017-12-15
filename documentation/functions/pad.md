# pad
## Prototype
```cpp
template<Raster>
padded_raster_view<Raster> pad(Raster raster
  , int leading_rows, int trailing_rows
  , int leading_cols, int trailing_cols
  , const typename traits<Raster>::value_type& pad_value);
```
  
## Description
Takes an input raster and return the same raster augmented with trailing rows and columns. The added rows and columns are not mutable, even when the input raster is. 

## Definition
[<blink/raster/padded_raster_view.h>](./../../include/blink/raster/padded_raster_view.h)

## Requirements on types
Raster must implement the RasterView concept

## Preconditions
leading_rows, trailing_rows, leading_cols and trailing_cols must be positive. The input raster must be initialized.

## Complexity
O(1)

## Example of use
```cpp//example_pad.cpp
#include <blink/raster/padded_raster_view.h>
#include <blink/raster/plot_raster.h>
#include <blink/raster/io.h>

namespace br = blink::raster;

int main()
{
  auto raster = br::create_temp<int>(3, 4);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }
  
  auto padded = br::pad(raster, 1, 4, 3, 2,-1);
  plot_raster(padded);
  return 0;
}
```

Output:
```
Rows: 8, Cols: 9, Value type: int
-1      -1      -1      -1      -1      -1      -1      -1      -1
-1      -1      -1      3       6       2       5       -1      -1
-1      -1      -1      1       4       0       3       -1      -1
-1      -1      -1      6       2       5       1       -1      -1
-1      -1      -1      -1      -1      -1      -1      -1      -1
-1      -1      -1      -1      -1      -1      -1      -1      -1
-1      -1      -1      -1      -1      -1      -1      -1      -1
-1      -1      -1      -1      -1      -1      -1      -1      -1
```

## Notes

## See also

