# any_raster
```cpp 
any_raster<T> 
```
## Description
 Type erasing wrapper around any RasterView with value type `T`

## Example of use
See documentation of the [make_any_raster](./../functions/make_any_raster.md) function.

## Definition
[<pronto/raster/any_raster.h>](./../../include/pronto/raster/any_raster.h)

## Template parameters
|Parameter|Description|Default|
|----------|--------|---------|
|T|The value type of the raster| - |

## Model of
RasterView.
The iterator and const_iterator types associated with the  `any_raster` class are conforming to the `RasterIterator` concept. The iterator types conform to the `RandomAccessible` concept if their underlying raster is `RandomAccessible`. (***in progress***)

## Type requirements
None

## Public base classes
None

## Members
`any_raster<T>` has all default constructors, assignment operators and destructors using reference semantics (with reference semantics is meant that the copy of an object gives access to the same data as the original object; the actual data is not copied.)

The following member functions implement the `RasterView` and `Subbable` concepts.
```cpp
any_raster<T>::const_iterator begin() const;
any_raster<T>::const_iterator end() const;
any_raster<T>::iterator begin() ;
any_raster<T>::iterator end() ;
int rows() const;
int cols() const;
int size() const;
any_raster<T> sub_raster(int start_row, int start_col, int rows, int cols) const;
```

## New Members
|Function|Effect|
|----------|--------|
|` template<class Raster>any_raster(Raster r)` |Constructs any_raster by wrapping around Raster r. Raster must be a model of RasterView. The following constructor let's any_raster be a type erased proxy of `Raster r`. Requires `traits<Raster>::value_type == T`.|

## Notes 
This type erasure is offered for two potential reasons. 
1. Type erasure can reduce the number of compiled functions and classes. Instead of compiling a function or class to work with each possible raster type, it only is necessary to compile one to work with the `any_raster` type. 
2. Type erasure can support run-time polymorphism. When functions are compiled to work with `any_raster` it is not necessary at compile-time to know what exact type is hidden under the 'any_raster' type.

Type erasure introduces an indirection that comes at a performance cost. Functions applied on an `any_raster` object will be slower than those applied on the original underlying raster.

See also the function `make_any_raster` that is used to create an `any_raster` based on a given raster.
