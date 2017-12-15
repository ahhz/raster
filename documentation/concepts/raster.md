# Raster 

## Summary
A `Raster` is a two-dimensional sequence of values, organised by a number of rows and a number of columns.

## Refinement of
`SizedRange` as proposed in the [Working Draft, C++ Extensions for Ranges](http://open-std.org/JTC1/SC22/WG21/docs/papers/2016/n4569.pdf).

## Associated types
The `iterator` and `const iterator` associated with a `Raster` implement `RasterIterator`.

## Notation
`R` is a type implementing the Raster concept.
`mr` is a mutable Raster object
`nmr` is a non-mutable raster object 
`iterator_type` is the iterator for a mutable Raster object
`const_iterator_type` is the iterator for a non-mutable Raster object

## Definition

## Valid expressions
The following expressions are in support of the `SizedRange` concept:

```cpp
auto it =  mr.begin();
auto it =  mr.end();

auto cit =  nmr.begin();
auto cit =  nmr.end();
auto i =  nmr.size(); 
```

The following expressions are additional requirements:
```cpp
i = nmr.rows();
i = nmr.cols();
sr = nmr.sub_raster(first_row, first_col, rows, cols);
```

The following expressions are used to inspect  traits:
```cpp
typename blink::raster::traits<R>::iterator;
typename blink::raster::traits<R>::const_iterator;
typename blink::raster::traits<R>::value_type;
typename blink::raster::traits<R>::reference;
typename blink::raster::traits<R>::sub_raster;
```

#### Expression Semantics
The `begin()` and `end()` functions return iterators that correspond to respectively the first and one-past-the-last element of the `Raster`. The order of iteration over elements in the Raster is row-by-row and within each row column-by-column.  The `size()`, `rows()` and `cols()` are used to query the dimensions of the the `Raster`, whereby size() == rows() * cols().  
The `sub_raster(first_row, first_col, rows, cols)` member function returns a `RasterView` (a Raster that does not own its data and hence is cheap to copy) that refers to a subset of the raster.