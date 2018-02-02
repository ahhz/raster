# transform

## Prototype
```cpp
template<class F, class... R>
transform_raster_view<F, R...> transform(F f, R... rasters)
```

## Description
Create a transform_view that applies the `Callable` `f` cell-by-cell to the corresponding elements in rasters.  The `Callable` cannot modify its input variable. The return_type is an non-mutable RasterView.

## Definition
[<pronto/raster/transform_raster_view.h>](./../../include/pronto/raster/transform_raster_view.h)

## Requirements on types
`F` is a callable that takes as many input arguments as the are input rasters. The type of the input arguments must correspond to the value_type of the respective rasters.  

## Preconditions
All rasters must have the same dimensions. The `Callable` `f` is passed by value and should therefore be trivially copyable.

## Complexity
O(1)

## Example of use
```
//example_transform.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/transform_raster_view.h>

namespace br = pronto::raster;

template<class T>
T join(const T& a, const T& b, const T& c)
{
  return 100 * a + 10 * b + c;
}

struct sum
{
  template<class T>
  T operator()(const T& a, const T& b, const T& c) 
  {
    return a + b + c;
  }
};

int main()
{
  auto product = [](int a, int b, int c) { return a * b * c;  };
  
  auto aa = br::create_temp<int>(4, 5);
  auto bb = br::create_temp<int>(4, 5);
  auto cc = br::create_temp<int>(4, 5);
  int i = 0;
  for (auto&& v : aa) {
    v = ((i += 3) %= 7);
  }

  for (auto&& v : bb) {
    v = ((i += 3) %= 7);
  }

  for (auto&& v : cc) {
    v = ((i += 3) %= 7);
  }

  auto join_abc = br::transform(join<int>, aa, bb, cc);
  auto sum_abc = br::transform(sum{}, aa, bb, cc);
  auto product_abc = br::transform(product, aa, bb, cc);

  plot_raster(join_abc);
  plot_raster(sum_abc);
  plot_raster(product_abc);

  return 0;
}
```
Output:
```
Rows: 4, Cols: 5, Value type: int
304     630     263     526     152
415     41      304     630     263
526     152     415     41      304
630     263     526     152     415

Rows: 4, Cols: 5, Value type: int
7       9       11      13      8
10      5       7       9       11
13      8       10      5       7
9       11      13      8       10

Rows: 4, Cols: 5, Value type: int
0       0       36      60      10
20      0       0       0       36
60      10      20      0       0
0       36      60      10      20
```

## Notes
The return type of the transform function is considered to be an implementation detail and can change in future. However, it will always be a model of RasterView.

## See also
