# open 
## Prototype
```cpp
template<class T> 
gdal_raster_view<T> open(const filesystem::path& path, access elem_access = read_write, int band_index = 1);
```

## Description
Opens a `GDALDataset` using the GDAL library and ties one raster band from the dataset to a new `gdal_raster_view` object. The user does not need to be concerned about closing the `GDALDataset` or `GDALRasterBand`. It will be closed when the `gdal_raster_view` or is deleted / goes out of scope.

The template parameter `T` is the value_type to which values of pixels in the `GDALRasterBand` are cast. This does not necessarily correspond to the data type as present in the raster band.

`elem_access` specifies the access type, it is either `read_only` or `read_write`.
`band_index` specifies which band from the dataset to open. The index is 1-based, so the first raster band has index 1.

## Definition
[<blink/raster/io.h>](./../../include/blink/raster/io.h)

## Requirements on types
The type of the raster dataset must be castable to `T` 

## Preconditions
path must refer to an existing raster dataset that can be read by the GDAL library. band_index must be greater than zero (the bands are numbered starting from 1). the raster dataset must at least have the number of bands corresponding to band_index.

## Complexity
The cost of the operation is governed by the filesystem that has to open the tiff file and read metadata. On opening it is not necessary to read the raster data itself.

## Example of use
```cpp
//example_open.cpp

#include <blink/raster/io.h>
#include <blink/raster/plot_raster.h>

namespace br = blink::raster;

int main()
{
  {
    auto raster = br::create<int>("test.tif", 3, 4, GDT_Byte);
  }
  auto opened_raster = br::open<short>("test.tif");
  int i = 0;
  for (auto&& v : opened_raster) {
    i = (i + 3) % 7;
    v = i;
  }
  plot_raster(opened_raster);
  return 0;
}
```

Output:
```

Rows: 3, Cols: 4.
3       6       2       5
1       4       0       3
6       2       5       1
```

## Notes
Note that these function open a GDALDataset for the given path, and there are some [limitations to opening GDALDatasets](http://www.gdal.org/gdal_8h.html#a6836f0f810396c5e45622c8ef94624d4). In particular, if you open a dataset object with read_write access, it is not recommended to open a new dataset on the same underlying file. The opened dataset, and hence the gdal_raster_view should only be accessed by one thread at a time. If you want to use it from different threads, you must add all necessary code (mutexes, etc.) to avoid concurrent use of the object.

## See also
[open](./open_and.md)
