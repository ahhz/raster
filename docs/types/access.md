# access (enum)
```cpp
 enum access
{
  read_only,
  read_write
};
```

## Description
Enum used to specify type of data access. 

## Example of use
The pronto::raster::access enum is used as an argument for the [open](/functions/open.md) function.

```cpp
namespace pr = pronto::raster;
auto ras = pr::open<int>("demo.tif", pr::access::read_only);

```

## Definition
<pronto/raster/access.h> [(open in Github)](https://github.com/ahhz/raster/blob/master/include/pronto/raster/access.h)

## Template parameters
n.a.

## Model of
n.a.

## Type requirements
n.a.

## Public base classes
n.a.

## Members
n.a.

## New Members
n.a.

## Notes 
