# any_blind_raster
```cpp
#include <blink/raster/any_blind_raster.h>
```
```cpp
class any_blind_raster;
```
`any_blind_raster` is a type erased `any_raster<T>`, to allow the `value_type` to be resolved at run-time. This is relevant to this library because the value type of rasters is sometimes only known at runtime (consider for instance a raster being read from file). `any_blind_raster` does not apply comform to the `RasterView` concept because it does not have begin() and end() member function. It does have the following member functions

```cpp
template<class T> any_blind_raster(any_raster<T> raster);
int rows();
int cols();
int size();
any_blind_raster sub_raster(int first_row, int first_col, int nrows, int ncols);
template<class T> any_raster<T> get_by_type();
template<std::size_t Index> any_raster<...> get_by_index();
std::size_t get_index();
```

See also following functions, to apply a function on an any_blind_raster:
```cpp
 template<class F>
    auto blind_function(F f, any_blind_raster r)
   
```
and to create an any_blind_raster from a given raster.
```cpp
 template<class Raster>
    any_blind_raster make_any_blind_raster(Raster r)
```
