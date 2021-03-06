# create_temp_from_model

## Prototype
There are two prototypes for this function. The first lets the user specify the `GDALDataType` for the created dataset. The second uses a `GDALDatatype` that matches template parameter `T`.
```cpp
template<T, U>
gdal_raster_view<T> create_temp_from_model(gdal_raster_view<U> model);

template<T, U>
gdal_raster_view<T> create_temp_from_model(gdal_raster_view<U> model, GDALDataType data_type);
```
## Description
Uses the GDAL library to create a single-band raster dataset at a temporary file location. The gdal_raster_view that is used as the model provides the number of rows and columns as well as the coordinate system and affine transformation. All other creation settings are fixed. This means:
- the dataformat is GTiff
- the file is tiled
- the block size is 256 x 256
- the compression option is set to deflate
- the interleave option is set to band  

## Definition
<pronto/raster/io.h> [(open in Github)](https://github.com/ahhz/raster/blob/master/include/pronto/raster/io.h)

## Requirements on types
If the data_type parameter is not used, T must be one of the [supported data types](./../types/gdal_data_type.md) ( unsigned char, unsigned int, int, float, double).
If the data_type parameter is used it must be corresponding to one of the supported data types, AND the associated data type must be castable to T.

## Preconditions
 The model must be associated with GDALRasterBand, i.e. it cannot be a default constructed gdal_raster_view because that does not have a projection of geotransform associated with it. When used, the data_type parameter must correspond to one of the [supported data types](./../types/gdal_data_type.md).

## Complexity
The cost of the operation is governed by the filesystem that has to create the tiff file. 

## Example of use

```cpp
//example_create_temp_from_model.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>

namespace pr = pronto::raster;

int main()
{
  auto model = pr::create<int>("test.tif", 3, 4);

  auto output = pr::create_temp_from_model<float>(model);
  float i = 0.7f;
  for (auto&& v : output) {
    i = i * (1 - i);
    v = i;
  }
  plot_raster(output);
  return 0;
}
```

Output:
```

Rows: 3, Cols: 4.
0.21    0.1659  0.138377        0.119229
0.105013        0.0939856       0.0851523       0.0779014
0.0718328       0.0666728       0.0622276       0.0583553
```
## Notes
When no affine transformation is specified in the model, a default affine transformation is assumed, such that the cell size is 1, the upperleft cell is centred at {0,0} and the positive vertical direction is downwards. This is consistent with the default affine transformation of ArcGIS, and different from GDAL's.
The sub_raster of a gdal_raster_view is also a gdal_raster_view and will provide the correct geotransform for its extent.

## See also
[create](./create.md),create_temp](./create_temp.md), [create_from_model](./create_from_model.md)
