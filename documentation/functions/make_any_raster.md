# make_any_raster
## Prototype
```cpptemplate
template<class Raster> 
any_raster<typename traits<Raster>::value_type> make_any_raster(Raster raster);
```
## Description
Create a type erased any_raster for a a given `Raster`. 

## Definition
[<blink/raster/any_blind_raster.h>](./../../include/blink/raster/any_blind_raster.h)

## Requirements on types
`Raster` must implement the `RasterView` concept.

## Preconditions
`raster` must be an initialized raster 

## Complexity
O(1)

## Example of use

```cpp
//example_any_raster.cpp

#include <blink/raster/any_raster.h>
#include <blink/raster/io.h>
#include <blink/raster/offset_raster_view.h>
#include <blink/raster/plot_raster.h>

namespace br = blink::raster;

// Here any_raster is used to implement a recursive function. Without 
// using the type erasure of any_raster it would be impossible to 
// determine the return type.

template<class T>
br::any_raster<T> recursive_function(br::any_raster<T> r)
{
  // continue offsetting by 1 cell until the corner cell is > 4 

  int pad_value = 999; // > 4 to guarantee that the recursion will end
  
  auto offset_view = br::offset(r, 1, 1, pad_value);
  if (*offset_view.begin() > 4) {
    return br::make_any_raster(offset_view);
  }
  else {
    return recursive_function(br::make_any_raster(offset_view));
  }
}

int main()
{
  auto raster = br::create_temp<int>(5, 4);

  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }

  auto result = recursive_function(br::make_any_raster(raster));

  plot_raster(raster);
  plot_raster(result);

  return 0;
}
```

Output:
```
Rows: 5, Cols: 4, Value type: int
3       6       2       5
1       4       0       3
6       2       5       1
4       0       3       6
2       5       1       4

Rows: 5, Cols: 4, Value type: int
5       1       999     999
3       6       999     999
1       4       999     999
999     999     999     999
999     999     999     999
```
## Notes
Using the `any_raster` object will be less efficient than `Raster`. 
An example of a context where `any_raster` is useful is the map algebra functionality in this library. Using `any_raster` the return type of an operation on two `any_raster` object can again be an `any_raster` object. It thus becomes possible to nest operations without nesting template instantiations. It also becomes possible to specify operations at run-time, instead of compile time.

## See also