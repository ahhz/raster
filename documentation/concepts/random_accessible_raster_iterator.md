# RandomAccessibleRasterIterator
## Summary
An iterator for a `RasterView` object, with random access functionality. It shares most of the requirements of [RandomAccessIterator](http://en.cppreference.com/w/cpp/concept/RandomAccessIterator), but for the the `reference` type only requires that it implements `ProxyReference` and not that it is a true reference.
## Refinement of
RasterIterator
## Associated types
The `reference` type of `RasterIterator` implements the `ProxyReference` concept.
## Notation
`iterator_type` implements the `RandomAccessibleRasterIterator` concept.
`i` and `j` are iterator_type object.
`v` is an object of `std::iterator_traits<iterator_type>::value_type`
`n` is an integer of type std::iterator_traits<iterator_type>::value_type`
## Definitions
## Valid expressions
```cpp
iterator_type i;
v = *i;
j = i;
j = ++i;
j = i++;
i += n;
i -= n;
j = i--;
j = --i;
bool check = i == j;
bool check = i != j;
v = i[n];
bool check = i <= j;
bool check = i >= j;
bool check = i < j;
bool check = i > j;
```

For mutable iterators
```cpp
*i = v;
*i += v;  // and all other modifying assignments as supported by the ProxyReference requirements.
```

## Expression Semantics
As those of RandomAccessIterator and RasterIterator.

## Notes
The iterators in the Raster library do generally not conform to the standard [RandomAccessIterator](http://en.cppreference.com/w/cpp/concept/RandomAccessIterator) concept, even when they implement all of the requirements related to random access. This is because the reference type of the Raster iterators typically is a ProxyReference, but not an actual reference to the value_type as required by RandomAccessIterator. This is a [well-documented](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2003/n1550.htm) limitation of the C++ standard iteration concepts, that hopefully will be resolved in the near future. 
In the meanwhile, we use the RandomAccessibleRasterIterator concept the specify the behaviour of raster iterators that meet the requirements of RandomAccessIterator, but do not necessarily meet the requirement that the reference type is a true reference to the value type.