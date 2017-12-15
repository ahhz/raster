# traits<Raster>
## Description
Traits class to inspect properties of a `Range`.
## Example of use
## Definition
## Template parameters
|Parameter|Description|Default|
|----------|--------|---------|
|Raster|The raster type of which to inspect properties|n.a.|
## Model of
N.A.
## Type requirements
|Parameter|Requires|
|----------|--------|---------|
|Raster|Model of Raster concept. Model of Subbable concept depending on use|

## Public base classes
N.A.
## Members
The following traits are supported.


|trait|corresponds to|
|`traits<Raster>::sub_raster`|decltype(r.sub_raster(int(), int(), int(),int())|
|`traits<Raster>::iterator`| decltype(std::begin(r))|
|`traits<Raster>::const_iterator`|decltype(std::cbegin(r))|
|`traits<Raster>::value_type`|std::iterator_traits<traits<Raster>::iterator>::value_type|
|`traits<Raster>::reference`|std::iterator_traits<traits<Raster>::iterator>::reference|
## New Members
## Notes 

```cpp
#include <blink/raster/traits.h>
```
The traits struct is used to introspect properties of a `Raster`. The following traits are supported.

```cpp
traits<Raster>::iterator;
traits<Raster>::const_iterator;
traits<Raster>::value_type;
traits<Raster>::reference;
traits<Raster>::sub_raster;
```