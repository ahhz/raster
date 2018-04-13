# optionalize

## Prototype
```cpp
template<class Raster>
optional_raster_view<Raster> optionalize(Raster raster)
```
## Description
Takes a raster view with value_type input_value_type as input and returns a raster view of to the same values but wrapped as an optional<input_value_type>. The resulting raster is non-mutable, even if the input raster is. 

## Definition
<pronto/raster/optional_raster_view.h> [(open in Github)](https://github.com/ahhz/raster/blob/master/include/pronto/raster/optional_raster_view.h)

## Requirements on types
Raster must implement the RasterView concept. 

## Preconditions
`raster` must be initialized
 
## Complexity
O(1)

## Example of use

```cpp
//example_optional_raster_view.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/optional.h>
#include <pronto/raster/optional_raster_view.h>
#include <pronto/raster/padded_raster_view.h>
#include <pronto/raster/plot_raster.h>

namespace pr = pronto::raster;

int main()
{
  auto raster = pr::create_temp<int>(3, 4, GDT_Byte);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }
  auto optional_raster = pr::optionalize(raster);
  auto padded_optional_raster = pr::pad(optional_raster, 2, 2, 2, 2, pr::none);
  
  plot_raster(raster);
  plot_raster(optional_raster);
  plot_raster(padded_optional_raster);
  
  return 0;
}
```
Output:
```
Rows: 3, Cols: 4, Value type: int
3       6       2       5
1       4       0       3
6       2       5       1

Rows: 3, Cols: 4, Value type: class std::optional<int>
3       6       2       5
1       4       0       3
6       2       5       1

Rows: 7, Cols: 8, Value type: class std::optional<int>
-       -       -       -       -       -       -       -
-       -       -       -       -       -       -       -
-       -       3       6       2       5       -       -
-       -       1       4       0       3       -       -
-       -       6       2       5       1       -       -
-       -       -       -       -       -       -       -
-       -       -       -       -       -       -       -
```
## Notes

## See also

