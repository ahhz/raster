# RasterIterator

## Summary
An iterator for a `Raster` object.

## Refinement of
[`InputIterator`](http://en.cppreference.com/w/cpp/concept/InputIterator) and/or [`OutputIterator`](
http://en.cppreference.com/w/cpp/concept/OutputIterator) 
`MultiPassIterator`

## Associated types
The `reference` type of `RasterIterator` implements the `ProxyReference` concept. The iterator of a `Raster` object must implement `RasterIterator`.

## Notation
`iterator_type` implements the `RasterIterator` concept.
`i` and `j` are iterator_type object.
`v` is an object of `std::iterator_traits<iterator_type>::value_type`

## Definitions

## Valid expressions
For all iterators:
```cpp
iterator_type i;
v = *i;
j = i;
j = ++i;
j = i++;
bool check = i == j;
bool check = i != j;
```

For mutable iterators
```cpp
*i = v;
*i += v;  // and all other modifying assignments as supported by the ProxyReference requirements.
```

## Expression semantics
The iterator iterates over the elements of the raster row-by-row and within each row column-by-columns. Otherwise the semantics follow those of `InputIterator` and `OutputIterator`.

## Complexity guarantees
Not applicable

## Invariants
Not applicable

#### Models
Not applicaable

## Notes
Most standard iterator concepts require the `reference` type to be equal to `value_type&` and hence iterators that offer functionality beyond the requirements listed here are still only `InputIterator` or `OutputIterators`. Hopefully future amendments to the standard will make it able to express more functionality in terms of standard iterator concepts.
