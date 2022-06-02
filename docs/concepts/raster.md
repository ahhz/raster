# Raster 

## Summary
A `Raster` is a `range` that iterates over the elements in a two-dimensional raster.

## Refinement of
`sized_range` and `view` as defined in the [C++20 Ranges library ](https://en.cppreference.com/w/cpp/ranges). 

## Associated types
The `iterator` associated with a `raster` implements `raster_iterator`.

## Notation
`R` is a class that confrims to the `Raster` concept.
`ras` is a object that confirms to the `Raster` concept.

## Definition

## Valid expressions
The following expressions are additional to those of `sized_range`:
```cpp
i = ras.rows();
i = ras.cols();
sr = ras.sub_raster(first_row, first_col, rows, cols);
```

The following expression is used to inspect traits:
```cpp
typename pronto::raster::traits<R>::sub_raster;
```

#### Expression Semantics
The order of iteration over elements in a `raster` is row-by-row and within each row column-by-column.  The  `rows()` and `cols()` are used to query the dimensions of the the `raster`, whereby `size() == rows() * cols()`.  
The `sub_raster(first_row, first_col, rows, cols)` member function returns a `raster` that refers to a subset of the raster. To date, all rasters in the library have a `sub_raster` type that is identical to the raster, but it is not a requirement. 