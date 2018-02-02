# optional_to_nodata

## Prototype
```cpp
template<class Raster>
transform_raster_view<optional_to_nodata_functor<value_type>, Raster>
  optional_to_nodata(Raster raster, const value_type& nodata_value);

where value_type is  shorthand for: 
recursive_optional_value_type<typename traits<Raster>::value_type > >;
```

## Description
Creates a new raster view that removes the nodata wrapper of the input values and replaces any uninitialized values with the nodata_value.The optional wrapper is removed recursively, so values of optional<optional<T> > are transformed to values of type T.

## Definition
[<pronto/raster/nodata_transform.h>](./../../include/pronto/raster/nodata_transform.h)

## Requirements on types
`Raster` must implement the RasterView concept.

## Preconditions

## Complexity
O(1)

## Example of use

```cpp
//example_8.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/nodata_transform.h>
#include <pronto/raster/plot_raster.h>

namespace br = pronto::raster;

int main()
{
  // Create some data
  auto in = br::create_temp<int>(4,5);
  int i = 0;
  for (auto&& v : in) {
    v = i;
    i = (i + 3) % 7;
  }

  // Treat value 6 as nodata
  auto nodata = br::nodata_to_optional(in, 6);

  // Treat nodata as value -99
  auto un_nodata = br::optional_to_nodata(nodata, -99);

  plot_raster(in);
  plot_raster(nodata);
  plot_raster(un_nodata);
  
  return 0;
}
```

Output: 
```
Rows: 4, Cols: 5, Value type: int
0       3       6       2       5
1       4       0       3       6
2       5       1       4       0
3       6       2       5       1

Rows: 4, Cols: 5, Value type: class std::optional<int>
0       3       -       2       5
1       4       0       3       -
2       5       1       4       0
3       -       2       5       1

Rows: 4, Cols: 5, Value type: int
0       3       -99     2       5
1       4       0       3       -99
2       5       1       4       0
3       -99     2       5       1

```

## Notes
Optional values are an idiomatic way of considering values that can potentially be absent. Nodata values are the idiomatic way of GIS to represent such values. The optional_to_nodata and nodata_to_optional function provide a translation between these two representations. Functions in the Pronto Raster library that ar "nodata aware" work on the basis of the optional wrapper. For example the raster algebra operations skip over elements where one of the input elements is missing. The moving window indicators skip over missing data. 

## See also
[nodata_to_optional](./nodata_to_optional.md)