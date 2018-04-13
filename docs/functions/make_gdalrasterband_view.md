# make_gdalrasterband_view

## Prototype
```cpp
template<T>
gdal_raster_view<T> make_gdalrasterdata_view(GDALRasterBand* band);

template<T>
gdal_raster_view<T> make_gdalrasterdata_view(std::shared_ptr<GDALRasterBand> band);
```

## Description
Creates a gdal_raster_view for the band. The user is responsible for the lifetime of the band, which must exceed that of the gdal_raster_view. The `make_gdalrasterband_view` functions are not generally recommended to access raster data. Generally, the [`open` function](./open.md) is preferred.

However, you may need to used the `make_gdalrasterdata_view` function in the following cases:
1. You need to interact with code that uses GDALRasterBand* already
2. You need to open multiple bands from the same dataset with read_write access. 
In the latter case, you can use two otherwise undocumented functions that open a GDALDataset and GDALRasterBand as a smart pointer. The dataset will be closed when the (last copy of) the shared_ptr is deleted.

```cpp
std::shared_ptr<GDALDataset> detail::open_dataset(filesystem::path path, access elem_access);
std::shared_ptr<GDALRasterBand> detail::open_band(std::shared_ptr<GDALDataset> dataset, int band_index);
```
## Definition
<pronto/raster/io.h> [(open in Github)](https://github.com/ahhz/raster/blob/master/include/pronto/raster/io.h)

## Requirements on types
The native value type of the GDALRasterBand must be castable to T. 

## Preconditions
band must point to a valid and GDALRasterBand

## Complexity
Constant cost, independent of raster size

## Example of use
```cpp
//example_make_gdalrasterdata_view.cpp

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
  // open dataset separately from band 
  std::shared_ptr<GDALDataset> dataset = pr::detail::open_dataset("test.tif", pr::read_only);
  std::shared_ptr<GDALRasterBand> band  = pr::detail::open_band(dataset, 1);

  // Note that here the type must be specified
  auto view = pr::make_gdalrasterdata_view<int>(band);

  plot_raster(view);
  return 0;
}
```

## Notes
## See also
[`open`](./open.md)

