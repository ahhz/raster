# uniform
## Prototype
```cpp
template<class T>
uniform_raster<T> uniform(int rows, int cols, const T& value)
```
## Description
Creates a uniform raster view of given dimensions and value. The raster is non-mutable. The results raster implements the RasterView concept.

## Definition
[<pronto/raster/uniform_raster_view.h>](./../../include/pronto/raster/uniform_raster_view.h)

## Requirements on types
T must be copy constructible.

## Preconditions
rows and cols must be greater than 0.

## Complexity
O(1), the same complexity as copying a object of type T.

## Example of use
```cpp
//example_uniform.cpp

#include <pronto/raster/plot_raster.h>
#include <pronto/raster/uniform_raster_view.h>


namespace br = pronto::raster;

int main()
{
  auto raster = br::uniform(5, 6, 3.14);
  plot_raster(raster);
  return 0;
}
```
Output:

```
Rows: 5, Cols: 6, Value type: double
3.14    3.14    3.14    3.14    3.14    3.14
3.14    3.14    3.14    3.14    3.14    3.14
3.14    3.14    3.14    3.14    3.14    3.14
3.14    3.14    3.14    3.14    3.14    3.14
3.14    3.14    3.14    3.14    3.14    3.14
```

## Notes

## See also
