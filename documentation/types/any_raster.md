# any_raster
```cpp
#include <blink/raster/any_raster.h>
```
```cpp
template<class T> class any_raster;
```

`any_raster<T>` implements the is a type erased view for a raster with `value_type` `T`. This type erasure is offered for two potential reasons. 
1. Type erasure can reduce the number of compiled functions and classes. Instead of compiling a function or class to work with each possible raster type, it only is necessary to compile one to work with the `any_raster` type. 
2. Type erasure can support run-time polymorphy. When functions are compiled to work with `any_raster` it is not necessary at compile-time to know what exact type is hidden under the 'any_raster' type.

Type erasure introduces an indirection that comes at a performance cost. Functions applied on an `any_raster` object may be 2-3 times slower than those applied on the original underlying raster.

The iterator and const_iterator types associated with the  `any_raster` class are conforming to the `RasterIterator` concept. The iterator types conform to the `RandomAccessible` concept if their underlying raster is `RandomAccessible`. (***in progress***)

The `any_raster` class has the following member functions (along with default constructors, and assignment operators).
The following constructor let's any_raster be a type erased proxy of `Raster r`. Requires `traits<Raster>::value_type == T`.

```cpp  
template<class Raster>
any_raster(Raster r);
```
The following member functions implement the `RasterView` and `Subbable` concepts.
```cpp
any_const_iterator<T> begin() const;
any_const_iterator<T> end() const;
any_iterator<T> begin() ;
any_iterator<T> end() ;
int rows() const;
int cols() const;
int size() const;
any_raster<T> sub_raster(int start_row, int start_col, int rows, int cols) const;
```
See also the function `make_any_raster` that is used to create an `any_raster` based on a given raster.
