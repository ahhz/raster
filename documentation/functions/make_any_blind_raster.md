# make_any_blind_raster
## Prototype
```cpp
template<class Raster>
any_blind_raster make_any_blind_raster(Raster raster)
```
## Description
Creates an any_blind_raster that wraps around a raster. This is a form of type_erasure as the value_type of a RasterView is known at compile time, but the value_type of an any_blind_raster is only known at run-time.

## Definition
[<pronto/raster/any_blind_raster.h>](./../../include/pronto/raster/any_blind_raster.h)

## Requirements on types
Raster implements the RasterView concept.

## Preconditions
The value type of the input Raster is one of any_blind_raster supported types: bool, short, unsigned int, int, unsigned short, unsigned char, float, double

## Complexity
O(1) 

## Example of use
```cpp
//example_any_blind_raster.cpp

#include <pronto/raster/any_blind_raster.h>
#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/transform_raster_view.h>

namespace pr = pronto::raster;

// the following function shows how any_blind_raster is used to decide 
// at runtime which value_type to use

template<class Raster> 
pr::any_blind_raster optionally_take_square_root(Raster raster, bool take_root)
{
  using input_value_type = typename pr::traits<Raster>::value_type;
  if (take_root) {

    // value type will be return type of sqrt<input_value_type>

    auto square_rooted = pr::transform(std::sqrt<input_value_type>, raster);
    return pr::make_any_blind_raster(square_rooted);
  }
  else {

    // value_type will be traits<Raster>::value_type

    return pr::make_any_blind_raster(raster);
  }
}

int main()
{
  auto raster = pr::create_temp<int>( 3, 4, GDT_Byte);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }

  pr::any_blind_raster a = optionally_take_square_root(raster, false);
  pr::any_blind_raster b = optionally_take_square_root(raster, true);

  plot_raster(a);
  plot_raster(b);

  return 0;
}
```
Output: 

```
Rows: 3, Cols: 4, Value type: int
3       6       2       5
1       4       0       3
6       2       5       1

Rows: 3, Cols: 4, Value type: double
1.73205 2.44949 1.41421 2.23607
1       2       0       1.73205
2.44949 1.41421 2.23607 1
```


## Notes
Accessing the values in an any_blind raster is slower than accessing values in the original raster (factor ~2).
 
## See also
[any_blind_raster](./../types/any_blind_raster.md), [blind_function](./blind_function.md)

