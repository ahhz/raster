# pair_raster_view
```cpp
#include <blink/raster/pair_raster_view.h>
```
```cpp
template<class R1, class R2> class pair_raster_view;
```

 
The `pair_raster_view` class presents a form of zip-iteration for two same-sized rasters, each comforming to the `RasterView` concept. The `value_type` of the `tuple_raster_view` is a `std::pair` of  the `value_type`'s of the two rasters. The `reference` type is a `std::pair` of  the `reference` types of the two rasters. 

Both `R1` and `R2` are required to conform to the `RasterView` concept. 
The `pair_raster_view` is `Subbable` if `R1` and `R2` are `Subbable`.
The `pair_raster_view` is `RecursivelySubbable` if all of `R1` and `R2` are `RecursivelySubbable`.

The iterator and const_iterator types associated with the  `pair_raster_view` class are conforming to the `RasterIterator` and `RandomAccessible` concepts.

The pair_raster_view  has the following public interface (along with default constructors, and assignment operators).
```cpp
   pair_raster_view(const R1& r1, const R2& r2); 
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
   
pair_raster_view::sub_raster_type sub_raster(int start_row, int start_col, int rows, int cols);
```
See also the function `raster_pair` that is used to create a `pair_raster_view` for two rasters.
