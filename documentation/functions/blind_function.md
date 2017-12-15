# make_any_blind_raster
## Prototype
```cpp
template<class Function>
auto blind_raster(Function function, any_blind_raster raster) ->decltype(function(any_raster<int>));
```
## Description
Apply a function on the raster wrapped by any_blind_raster.

## Definition
[<blink/raster/any_blind_raster.h>](./../../include/blink/raster/any_blind_raster.h)

## Requirements on types
Function must be `Callable` with with `any_raster<T>` as the input argument, whereby `T` must be a supported type((`bool`, `short`, `unsigned int`, `int`, `unsigned short`, `unsigned char`, `float`, `double`). The return type of the `Function` Callable must be independent of `T`.

## Preconditions
The input raster must be initialized, i.e. not be in the default-constructed state.
## Complexity
The complexity is that of `function(any_raster<T>)` 

## Example of use
```cpp

//example_blind_function.cpp
#include <blink/raster/raster.h>
#include <blink/raster/any_blind_raster.h>

namespace br = blink::raster;

struct square
{
public:
  template<class T>
  br::any_blind_raster operator()(const br::any_raster<T>& raster) const
  {
    auto sq_fun = [](const T& v) {return v * v; };
    auto result_typed = br::transform(sq_fun, raster);

    // wrapping as any_blind_raster because the return type must be 
    // independent of T.
    return br::make_any_blind_raster(result_typed);
  }
};

int main()
{
  // prepare a raster in separate scope
  {
    auto raster = br::create<int>("demo.tif", 3, 4, GDT_Byte);
    auto i = 0;
    for (auto&& v : raster) {
      i = (i + 3) % 7;
      v = i;
    }
  }

  // Open without specifying the type of the raster
  br::any_blind_raster ras = br::open_any("demo.tif");
  br::any_blind_raster ras_sq = br::blind_function(square{}, ras);
  plot_raster(ras);
  plot_raster(ras_sq);
  return 0;
}

```
Output:
```
Rows: 3, Cols: 4, Value type: unsigned char
3       6       2       5
1       4       0       3
6       2       5       1

Rows: 3, Cols: 4, Value type: int
9       36      4       25
1       16      0       9
36      4       25      1
```

## Notes
 
## See also
[any_blind_raster](./../types/any_blind_raster.md),[make_any_blind_raster](./make_any_blind_raster.md), [open_any](./open_any.md), [blind_function](./blind_function.md)

