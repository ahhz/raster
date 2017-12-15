# transform_raster_view
```cpp
#include <blink/raster/transform_raster_view.h>
```
```cpp
template<class F, class... R> class transform_raster_view;
```

A `transform_raster_view<F, R...>` is a `RasterView` that associates a function (more precisely a [Callable](http://en.cppreference.com/w/cpp/concept/Callable) with a series of same-sized rasters. The `value_type` of each element is the result of the function applied on each of the corresponding element in the series of rasters. The series must consist of at least one raster.

Each of `R...` is required to conform to the `RasterView` concept. 
The `transform_raster_view` is `Subbable` if all of `R...` are `Subbable`.
The `transform_raster_view` is `RecursivelySubbable` if all of `R...` are `RecursivelySubbable`.

The iterator and const_iterator types associated with the  `transform_raster_view` class are conforming to the `RasterIterator` and `RandomAccessible` concepts. 

The `transform_raster_view` has the following member functions (along with default constructors, and assignment operators).

```cpp  
template<class ForwardingF>
transform_raster_view(ForwardingF&& f, R... rasters);
```
The following member functions implement the `RasterView` and `Subbable` concepts.
```cpp
iterator begin() const;
iterator end() const;
int rows() const;
int cols() const;
int size() const;
sub_raster_type sub_raster(int start_row, int start_col, int rows, int cols) const;
```
See also the function `transform` that is used to create a `transform_raster_view` for a given function and set of rasters.