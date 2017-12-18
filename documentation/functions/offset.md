# offset
## Prototype
```cpp
template<class Raster>
offset_raster_view<Raster> offset(Raster raster, int row_offset, int col_offset, const typename traits<Raster>::value_type& pad_value); 
```
where `offset_raster_view` is defined by a typedef:
```cpp
template<class Raster>
using offset_raster_view = typename padded_raster_view<typename traits<Raster> >::sub_raster
```

## Description
The `offset` function takes a raster as input and returns a raster of the same dimensions and with the same values but offset by a fixed number of rows and columns. For those cells where there is no offset coordinate is outside the input raster a pad_value will be used. 

## Definition
[<blink/raster/offset_raster_view.h>](./../../include/blink/raster/offset_raster_view.h)

## Requirements on types
`Raster` must implement the `RasterView` concept.

## Preconditions
`raster` must be initialized.

## Complexity
O(1)

## Example of use
```cpp
//example_offset.cpp
#include <blink/raster/offset_raster_view.h>
#include <blink/raster/plot_raster.h>
#include <blink/raster/io.h>

namespace br = blink::raster;

int main()
{
  auto raster = br::create_temp<int>(5, 6);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }
  
  auto off = br::offset(raster, 2, 3, -1);
  plot_raster(raster);
  plot_raster(off);
  return 0;
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

Rows: 5, Cols: 6, Value type: int
6       2       5       -1      -1      -1
3       6       2       -1      -1      -1
0       3       6       -1      -1      -1
-1      -1      -1      -1      -1      -1
-1      -1      -1      -1      -1      -1
```
## Notes
The offset function is readily implemented in terms of the pad function and sub_raster member function.

## See also

