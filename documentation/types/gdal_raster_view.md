# gdal_raster_view<T>

## Description
A model of RasterView that gives access to the data of a `GDALRasterBand`. 

## Example of use
See, for instance, the documentation of [`open`](./../functions/open.md) of which the return type is a `gdal_raster_view<T>`.

## Definition
[<blink/raster/gdal_raster_view.h>](./../../include/blink/raster/gdal_raster_view.h0

## Template parameters
|Parameter|Description|Default|
|----------|--------|---------|
|T|The value type of the raster| - |

## Model of
`gdal_raster_view<T>` is a model of `RasterView`, `RecursivelySubbable`.
`gdal_raster_view<T>::indicator` is a model of `RasterIterator`, `Mutable` and `RandomAccessible`.

## Type requirements
|Parameter|Requirements|
|----------|--------|
|`T`|This is not necessarily identical to the value type stored in the dataset. Upon reading a cell value from a dataset it is cast to `T`. Before writing a value `T` to the dataset it is cast to the value type of the dataset. Hence, `T` must be castable from and to the native value type of the `GDALRasterBand`|

## Public base classes
None

## Members
`gdal_raster_view` has all default constructors, assignment operators and destructors using reference semantics (with reference semantics is meant that the copy of an object gives access to the same data as the original object; the actual data is not copied.)

The following member functions implement the `RasterView` concept.
```cpp
int rows() const; 
int cols() const;
int size() const; 
gdal_raster_view::iterator begin(); 
gdal_raster_view::iterator end();
gdal_raster_view::const_iterator begin() const;
gdal_raster_view::const_iterator end() const;
gdal_raster_view sub_raster(int first_row
  , int first_col, int num_rows, int num_cols) const;
```

## New Members
|Function|Effect|
|----------|--------|
|` gdal_raster_view(std::shared_ptr<GDALRasterBand> band)`| Construct a gdal_raster_view for `band`, with shared ownership of`band`|
|` gdal_raster_view(GDALRasterBand* band) `| Construct a gdal_raster_view for `band`. It is the responsibility of the caller to make sure that lifetime of `band` exceeds that of the `gdal_raster_view`. It is the responsibility of the caller to delete `band`|
|` std::shared_ptr<GDALRasterBand> get_band() const `|Access the GDALRasterBand|
|`  CPLErr get_geo_transform(double* padfTransform) const `|Get the geo_transform. This is similar to [GDALDataSet::GetGeotransform](http://www.gdal.org/classGDALDataset.html#a5101119705f5fa2bc1344ab26f66fd1d). The main difference is that for gdal_raster_view that refer a subset of the GDALRasterband the geotransform for the subset is returned. A minor difference is that for dataset with a missing geotransform the default of ArcGIS is used, rather than the default of GDAL. CPLErr is defined by GDAL. |

## Notes 
See also the documentation of [GDALRasterBand](http://www.gdal.org/classGDALRasterBand.html).

