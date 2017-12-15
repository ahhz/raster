# make_any_blind_raster
## Prototype
```cpp
template<class Raster>
any_blind_raster make_any_blind_raster(Raster raster)
```
## Description
Creates an any_blind_raster that wraps around a raster. This is a form of type_erasure as the value_type of a RasterView is known at compile time, but the value_type of an any_blind_raster is only known at run-time.

## Definition
[<blink/raster/any_blind_raster.h>](./../../include/blink/raster/any_blind_raster.h)

## Requirements on types
Raster implements the RasterView concept.

## Preconditions
The value type of the input Raster is one of any_blind_raster supported types: bool, short, unsigned int, int, unsigned short, unsigned char, float, double

## Complexity
O(1) 

## Example of use
```cpp
//example_any_blind_raster.cpp
#include <blink/raster/raster.h>
#include <blink/raster/any_blind_raster.h>

#include <random> // for distributions and generators


namespace br = blink::raster;

// the following function shows how any_blind_raster is used to decide 
// at runtime which value_type to use
template<class Raster> 
br::any_blind_raster optionally_take_square_root(Raster raster, bool take_root)
{
  using input_value_type = typename br::traits<Raster>::value_type;
  if (take_root) {
    // value type will be return type of sqrt<input_value_type>
    auto square_rooted = br::transform(std::sqrt<input_value_type>, raster);
    return br::make_any_blind_raster(square_rooted);
  }
  else {
    // value_type will be traits<Raster>::value_type
    return br::make_any_blind_raster(raster);
  }
}

// a function object that can apply on any_raster with any value_type
struct value_type_getter
{
  template<class T>
  std::string  operator()(const br::any_raster<T>& raster) const
  {
    return std::string(typeid(T).name());
  }
};

std::string get_value_type(br::any_blind_raster raster)
{
  return br::blind_function(value_type_getter{}, raster);
}

int main()
{
  // prepare a raster 
  auto raster = br::create_temp<int>( 3, 4, GDT_Byte);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }

  br::any_blind_raster a = optionally_take_square_root(raster, false);
  br::any_blind_raster b = optionally_take_square_root(raster, true);

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

