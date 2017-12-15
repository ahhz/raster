# gdal_raster_view
```cpp
#include <blink/raster/gdal_raster_view.h>
```
```cpp
template<class T> class gdal_raster_view;
```
`gdal_raster_view` wraps a `GDALRasterBand*` to make it conform to the `RecursivelySubbable` and `RasterView` concepts. The `value_type` of the `RasterView` is `T`. When `T` does not correspond to the `GDALDatatype` of the `GDALRasterBand`, the values are cast upon access.

The iterator and const_iterator types associated with the  `gdal_raster_view` class are conforming to the `RasterIterator` and `RandomAccessible` concepts.

See also the documentation of [GDALRasterBand](http://www.gdal.org/classGDALRasterBand.html).

`gdal_raster_view` has the default constructors and destructors with reference semantics. Furthermore it has the following constructors:
```cpp
gdal_raster_view(std::shared_ptr<GDALRasterBand> band)
```
Create a gdal_raster_view for `band`, with shared ownership of`band`.
```cpp
gdal_raster_view(std::shared_ptr<GDALRasterBand> band, int first_row, int first_col, int num_rows, int num_cols)
```
Create a gdal_raster_view for a subset of`band`, with shared ownership of`band`.

```cpp
gdal_raster_view(GDALRasterBand* band)
```
Create a gdal_raster_view for `band`. It is the responsibility of the caller to make sure that lifetime of `band` exceeds that of the `gdal_raster_view`. It is the responsibility of the caller to delete `band`.
```cpp
gdal_raster_view(GDALRasterBand* band, int first_row, int first_col, int num_rows, int num_cols) 
```

Create a gdal_raster_view for a subset of `band`. It is the responsibility of the caller to make sure that lifetime of `band` exceeds that of the `gdal_raster_view`. It is the responsibility of the caller to delete `band`.


Access the GDALRasterBand with the following function
```cpp
 std::shared_ptr<GDALRasterBand> get_band() const 
```
Get the geo_transform with the following function.
```cpp
 CPLErr get_geo_transform(double* padfTransform) const
```
This is similar to [GDALDataSet::GetGeotransform](http://www.gdal.org/classGDALDataset.html#a5101119705f5fa2bc1344ab26f66fd1d). The main difference is that for gdal_raster_view that refer a subset of the GDALRasterband the geotransform for the subset is returned. A minor difference is that for dataset with a missing geotransform the default of ArcGIS is used, rather than the default of GDAL.

The following member functions implement the RasterView concept.
```cpp
int rows() const 
int cols() const 
int size() const 
gdal_raster_view::iterator begin() 
gdal_raster_view::iterator end() 
gdal_raster_view::const_iterator begin() const
gdal_raster_view::const_iterator end() const
gdal_raster_view::gdal_raster_view sub_raster(int first_row
  , int first_col, int num_rows, int num_cols) const
```