# create

## Prototype
There are two prototypes for this function. The first lets the user specify the `GDALDataType` for the created dataset. The second uses a `GDALDatatype` that matches template parameter `T`.

```cpp
template<class T>
gdal_raster_view<T> create(const filesystem::path& path, int rows, int cols, GDALDataType data_type);
  
template<class T>
gdal_raster_view<T> create(const filesystem::path& path, int rows, int cols);
```

## Description
Uses GDAL to create a single-band raster dataset of given dimensions at a file location specified by the `path` parameter. The user specifies the dimensions and datatype of the raster, but all other creation settings are fixed. This means:
- the dataformat is GTiff
- the file is tiled
- the block size is 256 x 256
- the compression option is set to deflate
- the interleave option is set to band   
  
## Definition
<pronto/raster/io.h> [(open in Github)](https://github.com/ahhz/raster/blob/master/include/pronto/raster/io.h)

## Requirements on types
If the data_type parameter is not used, `T` must be one of the [supported data types](./../types/gdal_data_type.md) ( unsigned char, unsigned int, int, float, double).
If the data_type parameter is used it must be corresponding to one of the supported data types, AND the associated data type must be castable to T.

## Preconditions
The path parameter must be a valid path and must include the desired file extension (i.e. ".tif" or ".tiff"). Rows and cols must be greater than 0. When used, the data_type parameter must correspond to one of the [supported data types](./../types/gdal_data_type.md). 

## Complexity
The cost of the operation is governed by the filesystem that has to create the tiff file. 

## Example of use
```cpp
//example_create.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>

namespace pr = pronto::raster;

int main()
{
  auto raster = pr::create<int>("test.tif", 3, 4, GDT_Byte);
  int i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }
  plot_raster(raster);
  return 0;
}
```
Output
```
Rows: 3, Cols: 4.
3       6       2       5
1       4       0       3
6       2       5       1
```

## Notes
If you would like to create raster data sets with different settings, use [GDAL](www.gdal.org) directly in conjunction with [make_gdalrasterband_view](./make_gdalrasterband_view.md).

## See also
[create_temp](./create_temp.md), [create_temp_from_model](./create_temp_from_model.md), [create_from_model](./create_from_model.md)
