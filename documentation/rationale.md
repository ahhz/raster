## Rationale
The purpose of this library is to facilitate working with geographical raster data using idiomatic C++, foremost the range based for-loop introduced in C++11. The advantage of that is that it becomes possible to use standard programming techniques and C++ libraries on raster data. The library is intended to especially useful for environmental modelling, including Cellular Automata modelling, spatial statistics and multi-scale analysis.

The central concept is RasterView; a RasterView object is a range with begin() and end() member functions that return iterators directing to the first cell of the associated raster and one-past-the-last. The order of iteration is row-by-row and within each row, column-by-column. Furthermore the RasterView concept has size(), rows() and cols() member functions to query the dimensions of the raster. The views are cheap to copy, so they can be passed by value.

All functions on raster views are lazy, using expression templates, unless otherwise indicated. For instance the transform and raster algebra functions do not perform the operations cell-by-cell directly, but return a raster view of which the cell values are only calculated when they are iterated over. 

### Accessing raster data
The library supports the following operations to access and transform raster data:

- open and create permanent or temporary geographical raster data sets
- expose raster data sets as raster views.
- access raster views cell-by-cell using standard conforming iterators and range-based for loops
- deal with nodata values gracefully by having `optional<T>` value types.
- apply type erasure on raster views to support runtime polymorphical operations


### Accessing spatial subsets and offsets
- create raster views that expose a subset (range of rows and columns) of other raster views
- create raster views that pad other raster views with leading and trailing rows and columns
- create raster views that expose other raster views at a given row and column offset
- create raster views that iterate over pairs of adjacent cells (horizontal and vertical edges)

### Raster algebra
The library supports the following operations to access and transform raster data:
- create raster views that apply any function on cell-by-cell on one or more raster views (transform function)
- create raster view that apply a function on a mix of raster data and non-raster (scalar) data
- Overload arithmatic(+,-, *, /, etc.) and logical (>, <, &&, ||, etc.) operators on  cell-by-cell operations on raster views.

### Moving windows
The library supports the following operations to access and transform raster data:
- create raster views that iterate over indicators calculated for subrasters centered on each cell
- create raster views that iterate over indicators calculated for circular windows centered on each cell
- create raster views that iterate over subraster views centered on each cell
