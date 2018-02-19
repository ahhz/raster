# raster_algebra_wrapper
```cpp
#include <pronto/raster/raster_algebra.h>
```
```cpp
template<class R> raster_algebra_wrapper;
```
`raster_algebra_wrapper` is a simple wrapper around either a `RasterView` object or a `any_blind_raster`. When wrapped around a `RasterView` the `raster_algebra_wrapper` is also a `RasterView` and all of the member functions simply forward to those of the wrapped object. The point of the raster_algebra_wrapper is to allow raster algebra operators and the `raster_algebra_transform` function to see which input arguments are raster and which are uniform. 

The `raster_algebra_wrapper` class has the following member functions (along with default constructors, and assignment operators).

```cpp
raster_algebra_wrapper(R raster);
```
Constructor to wrap around `raster`

```cpp
R unwrap();
```
The following member functions implement the `RasterView` and `Subbable` concepts, these are not available when `R == any_blind_raster`.
```cpp
const_iterator begin() const;
const_iterator end() const;
iterator begin() ;
iterator end() ;
int rows() const;
int cols() const;
int size() const;
sub_raster_type sub_raster(int start_row, int start_col, int rows, int cols) const;
```
## Overloaded binary operators for `raster_algebra_wrapper<R>`
```cpp
#include <pronto/raster/raster_algebra_operators.h>
```
Operators `+`, `-`, `*`, `/`, `%`, `&&`, `||`, `>`, `<`, `>=`, `>=`, `==` and `!=` are overloaded for operations on `raster_algebra_wrapper<R>` values. The following situations are considered, when applied as `a [operator] b`.

|Usage| type of a | type of b |
|-----|-----------|-----------|
|I|`raster_algebra_wrapper<R>`|`T`|
|II|`T`|`raster_algebra_wrapper<R>`|
|III|`raster_algebra_wrapper<R1>`|`raster_algebra_wrapper<R2>`|

### Usage I
Returns a raster with the same dimension as `a` that for every cell returns the value of the operator applied on the corresponding cell in `a` and the value of `b`.

### Usage II
Returns a raster with the same dimension as `b` that for every cells returns the value of the operator applied on the value of `a` and the corresponding cell in `b`.

### Usage III
Returns a raster with the same dimension as `a` and `b` that for every cell returns the value of the operator applied on the corresponding cell in `a` and the corresponding cell in `b`.

When the type of `R`, `R1` or `R2` is `any_blind_raster`, then the return type is  `raster_algebra_wrapper<any_blind_raster>`.
Otherwise, when the type of `R`, `R1` or `R2` is of the form `any_raster<...>` then the return type is of the form `raster_algebra_wrapper<any_raster<...> `>.
Otherwise the return type is of the form `raster_algebra_wrapper<transform_raster_view<...> >` .

The implementation of the operators uses 'optional filtering' which means that when either one or both of the arguments of the operator are of the form `optional<...>` then the result  of the operator is also of the form `optional<...>`. The result of the operator is initialized iff both arguments are initialized.

## Overloaded unary operators for `raster_algebra_wrapper<R>`
Operators `-` (negate) and `!` (logical not) are overloaded for operations on `raster_algebra_wrapper<R>` values. The operator return a raster that has the same dimensions as the argument and applies the operator on each element. 
When the type of `R` is `any_blind_raster`, then the return type is  `raster_algebra_wrapper<any_blind_raster>`.
Otherwise, when the type of `R` is of the form `any_raster<...>` then the return type is of the form `raster_algebra_wrapper<any_raster<...> `>.
Otherwise the return type is of the form `raster_algebra_wrapper<transform_raster_view<...> >` .

The implementation of the operators uses 'optional filtering' which means that when the arguments of the operator are of the form `optional<...>` then the result of  the operator is also of the form `optional<...>`. The result of the operator is initialized iff the argument is initialized.
