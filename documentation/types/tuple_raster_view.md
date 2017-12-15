#tuple_raster_view
```cpp
#include <blink/raster/tuple_raster_view.h>
```

```cpp
template<class... R> class tuple_raster_view;
```

The `tuple_raster_view` class presents a form of zip-iteration for a set of same-sized rasters, each comforming to the `RasterView` concept. The `value_type` of the `tuple_raster_view` is a `std::tuple` of all the `value_type`'s of the rasters. The `reference` type is a `std::tuple` of all the `reference` types of the rasters. 

Each of `R...` is required to conform to the `RasterView` concept. 
The `tuple_raster_view` is `Subbable` if all of `R...` are `Subbable`.
The `tuple_raster_view` is `RecursivelySubbable` if all of `R...` are `RecursivelySubbable`.

The iterator and const_iterator types associated with the  `tuple_raster_view` class are conforming to the `RasterIterator` and `RandomAccessible` concepts. 

The tuple_raster_view is has the following public interface (along with default constructors, and assignment operators).
```cpp
   tuple_raster_view(const R& ...r); 
```
The following member functions implement the `RasterView` and `Subbable` concepts.
```cpp
iterator begin(); 
iterator end(); 
const_iterator begin() const;
const_iterator end() const;
int rows() const;
int cols() const;
int size() const;
   
tuple_raster_view::sub_raster_type sub_raster(int start_row, int start_col, int rows, int cols);
```
See also the function `raster_tuple` that is used to create a `tuple_raster_view` for a set of rasters.