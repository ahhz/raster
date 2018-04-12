# raster_algebra_transform

## Prototype
```cpp
template<class F, class... Args>
auto raster_algebra_transform(F f, Args... args)
```

## Description
The arguments to the raster_algebra_transform can be a mix of rasters (wrapped in raster_algebra_wrapper) and other values. The result is a RasterView that iterates over all rasters and applies the `Callable` `f` taking as its arguments either the corresponding elements in rasters or the corresponding value.  The `Callable` cannot modify its input variable. The return_type is an non-mutable RasterView wrapped in a raster_algebra_wrapper, except when non of the input arguments is a raster, then the return type is the return type of F..

## Definition
<pronto/raster/raster_algebra_transform.h> [(open in Github)](https://github.com/ahhz/raster/blob/master/include/pronto/raster/raster_algebra_transform.h)

## Requirements on types
`F` is a callable that requires as many arguments as there are arguments in Args. For exposition, let's assume there are three arguments and F is a Callable such that : out_value =  f(in_value_1, in_value_2, in_value_3). Then Arg1 must either be in_value_type_1 or raster_algebra_wrap<Raster> of which the value_type is value_type_1.

## Preconditions
All rasters must have the same dimensions. The `Callable` `f` is passed by value and should therefore be trivially copyable.

## Complexity
O(1)

## Example of use
```
//example_raster_algebra_transform.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/raster_algebra_transform.h>

namespace pr = pronto::raster;

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
  
  auto a = pr::create_temp<int>(4, 5);
  auto b = pr::create_temp<int>(4, 5);
  auto c = 5;

  int i = 0;
  for (auto&& v : a) {
    v = ((i += 3) %= 7);
  }

  for (auto&& v : b) {
    v = ((i += 3) %= 7);
  }

  auto wrapped_a = pr::raster_algebra_wrap(a);
  auto wrapped_b = pr::raster_algebra_wrap(b);

  auto join_abc = pr::raster_algebra_transform(join<int>, wrapped_a, wrapped_b, c);
  auto sum_abc = pr::raster_algebra_transform(sum{}, wrapped_a, wrapped_b, c);
  auto product_abc = pr::raster_algebra_transform(product, wrapped_a, wrapped_b, c);
  
  plot_raster(join_abc);
  plot_raster(sum_abc);
  plot_raster(product_abc);

  return 0;
}
```
Output:
```
Rows: 4, Cols: 5, Value type: int
305     635     265     525     155
415     45      305     635     265
525     155     415     45      305
635     265     525     155     415

Rows: 4, Cols: 5, Value type: int
8       14      13      12      11
10      9       8       14      13
12      11      10      9       8
14      13      12      11      10

Rows: 4, Cols: 5, Value type: int
0       90      60      50      25
20      0       0       90      60
50      25      20      0       0
90      60      50      25      20
```

## Notes
The return type of the transform function is considered to be an implementation detail and can change in future. However, it will always be a model of RasterView.

## See also
