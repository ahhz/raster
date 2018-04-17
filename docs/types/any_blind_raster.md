# any_blind_raster
```cpp 
class any_blind_raster; 
```

## Description
Type erasing wrapper around an any_raster<T>, hiding the value_type `T` of the raster

## Example of use
See documentation of the [make_any_blind_raster](./../functions/make_any_blind_raster.md) function.

## Definition
<pronto/raster/any_blind_raster.h> [(open in Github)](https://github.com/ahhz/raster/blob/master/include/pronto/raster/any_blind_raster.h)

## Template parameters
n.a.

## Model of
The `any_blind_raster` partially implements the `RasterView` concept.

## Type requirements
n.a.

## Public base classes
None

## Members
`any_blind_raster` has all default constructors, assignment operators and destructors using reference semantics (with reference semantics is meant that the copy of an object gives access to the same data as the original object; the actual data is not copied.)

The following member functions implement the `RasterView` and `Subbable` concepts.
```cpp
int rows() const;
int cols() const;
int size() const;
any_blind_raster sub_raster(int start_row, int start_col, int rows, int cols) const;
```

## New Members
|Function|Effect|
|----------|--------|
|`template<class T>any_raster(any_raster<T> r)` |Constructs an any_blind_raster by wrapping around any_raster<T> r. |
|`template<class T> any_raster<T> get()`| returns the wrapped raster, requires `T` to be the value type of the wrapped raster (otherwise throws `bad_any_cast`).|
 

## Notes 
This type erasure is offered for one main reason:When raster data is read from file, we do not know necessarily what data type the files contain, at compile time. `any_blind_raster` hides the data type and can thus hold raster data with any data type. 

See also the function `make_any_blind_raster` that is used to create an `any_blind_raster` based on a given raster.

See also the function `blind_function` that applies a function on an `any_type_raster`.

The functions `open_any` and `export_any` are used to read and write raster data without having to specify the `value_type`.  

