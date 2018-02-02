# optionalize_function

## Prototype
```cpp
 template<class F>
    optional_filtered_function<F> optionalize_function(F f)
```

## Description
Takes a Callable f, and return another Callable that applies the same function, but treats optional input arguments differently. If any of the arguments are optional and uninitialized then the optional_filtered_function returns an uninitialized optional. Otherwise, the function return an initialized optional value derived from applying the Callable f on all the input arguments, whereby optional values are resolved to the underlying value. The function is included in the library in particular to be used in combination with the `transform`and `raster_algebra_transform` functions.

## Definition
[<pronto/raster/optional.h>](./../../include/pronto/raster/optional.h)

## Requirements on types
`F` must be a Callable.

## Preconditions

## Complexity
O(1)

## Example of use

```cpp
//example_optionalize_function.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/nodata_transform.h>
#include <pronto/raster/optional.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/transform_raster_view.h>

namespace br = pronto::raster;

int square(const int& v)
{
  return v * v;
}

int main()
{
  auto in = br::create_temp<int>(5, 4);
  int i = 0;
  for (auto&& v : in) {
    v = i;
    i = (i + 3) % 7;
  }

  // Treat value of 6 as nodata
  auto nodata_in = br::nodata_to_optional(in, 6);

  // Transform using square, skip over nodata values.
  auto nodata_in_sq = br::transform(br::optionalize_function(square), nodata_in);

  plot_raster(in);
  plot_raster(nodata_in);
  plot_raster(nodata_in_sq);

  return 0;
}
```

Output: 
```
Rows: 5, Cols: 4, Value type: int
0       3       6       2
5       1       4       0
3       6       2       5
1       4       0       3
6       2       5       1

Rows: 5, Cols: 4, Value type: class std::optional<int>
0       3       -       2
5       1       4       0
3       -       2       5
1       4       0       3
-       2       5       1

Rows: 5, Cols: 4, Value type: class std::optional<int>
0       9       -       4
25      1       16      0
9       -       4       25
1       16      0       9
-       4       25      1

```

## Notes

## See also
[nodata_to_optional](./nodata_to_optional.md), [optional_to_nodata](./optional_to_nodata.md), [transform](./transform.md)