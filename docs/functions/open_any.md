# open_any
## Prototype
```cpp
any_blind_raster open_any(const filesystem::path& path, access elem_access = read_write, int band_index = 1);
```
## Description
The `open_any` function opens the raster band in its native data type, which is only known at runtime, and subsequently wraps it as a `any_blind_raster` object.  Unlike the `open` function, here it is not necessary to specify the value_type of the opened raster. The raster is opened in its native format, i.e. based on its GDALDataType. `band_index` specifies which band from the dataset to open. The index is 1-based, so the first raster band has index 1
 
## Definition
[<pronto/raster/io.h>](./../../include/pronto/raster/io.h)

## Requirements on types
N.A.

## Preconditions
The parameter path must be a valid path to a raster dataset and the number of bands in the dataset must be at least equal to band_index. .

## Complexity
The cost of the operation is governed by the filesystem that has to open the tiff file and read metadata. On opening it is not necessary to read the raster data itself.

## Example of use
```cpp
//example_open_any.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>

namespace pr = pronto::raster;

int main()
{
  // prepare a file in a separate scope 
  {
    auto raster = pr::create<int>("test.tif", 3, 4, GDT_Byte);
    auto i = 0;
    for (auto&& v : raster) {
      i = (i + 3) % 7;
      v = i;
    }
  }
  // open without specifying that it is an integer type or GDT_Byte 

  pr::any_blind_raster abr = pr::open_any("test.tif");
  plot_raster(abr);
  return 0;
}
```
## Notes
The user does not need to be concerned about closing the `GDALDataset` or GDALRasterBand. It will be closed when the `any_blind_raster` object is deleted / goes out of scope.
 
Note that these function open a GDALDataset for the given path, and there are some [limitations to opening GDALDatasets](http://www.gdal.org/gdal_8h.html#a6836f0f810396c5e45622c8ef94624d4). In particular, if you open a dataset object with read_write access, it is not recommended to open a new dataset on the same underlying file. The opened dataset, and hence the gdal_raster_view should only be accessed by one thread at a time. If you want to use it from different threads, you must add all necessary code (mutexes, etc.) to avoid concurrent use of the object.

## See also
[open](./open.md)