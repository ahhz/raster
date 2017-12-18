# uniform_raster_view
```cpp
#include <blink/raster/uniform_raster_view.h>
```
```cpp
template<class T> class uniform_raster_view;
```
The `uniform_raster_view<T>` is a `non mutable, RecursivelySubbable` `RasterView` that has the same value for all cells. 

The iterator and const_iterator types associated with the  `uniform_raster_view` class are conforming to the `RasterIterator` and `RandomAccessible` concepts.

The `uniform_raster_view` has the following member functions (along with default constructors, and assignment operators).



```cpp
uniform_raster_view(int rows, int cols, const T& value);
```
The following member functions implement the RasterView concept.
```cpp
int rows() const 
int cols() const 
int size() const 
uniform_raster_view::iterator begin() const
uniform_raster_view::iterator end() const
uniform_raster_view sub_raster(int first_row
  , int first_col, int num_rows, int num_cols) const

```
See also the function `uniform` that is used to create a `uniform_raster_view` for a given dimension and value.
