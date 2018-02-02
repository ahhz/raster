### padded_raster_view
```cpp
#include <pronto/raster/padded_raster_view.h>
```
```cpp
template<class Raster> class padded_raster_view;
```
The `padded_raster_view` class implements the `RasterView` concept. It requires that `Raster` implements the `RasterView` concept. 
The `padded_raster_view<Raster>` provides a view of a `R` object augmented with leading and trailing rows and columns of a uniform value. `padded_raster_view<Raster>` objects are created by the `pad` function.

The original cells of the raster that is padded remain mutable if they were so. Cells in the leading and trailing rows and columns are not mutable.

The iterator and const_iterator types associated with the  `padded_raster_view` class are conforming to the `RasterIterator` and `RandomAccessible` concepts. (***in progress***)

If `Raster` is a `Subbable`, then so is `padded_raster_view<Raster>`.
If `Raster` is `RecursivelySubbable`, then so is `padded_raster_view<Raster>`.
`padded_raster_view` has the default constructors and destructors with reference semantics. Furthermore it has the following constructors:
```cpp
padded_raster_view(const Raster& raster,
        int leading_rows, int trailing_rows, 
        int leading_cols, int trailing_cols, const value_type& value) 
```

The following member functions implement the RasterView concepts.
```cpp
int rows() const 
int cols() const 
int size() const 
padded_raster_view::iterator begin() 
padded_raster_view::iterator end() 
padded_raster_view::const_iterator begin() const
padded_raster_view::const_iterator end() const
padded_raster_view::sub_raster_type sub_raster(int first_row
  , int first_col, int num_rows, int num_cols) const
```

See also the function `pad` that is used to create a `padded_raster_view`.
