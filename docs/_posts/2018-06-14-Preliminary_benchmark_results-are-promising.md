---
title:  "Preliminary benchmark results are promising"
date:   2018-06-14 12:00:00 -0000
---
Benchmarking of the Pronto Raster library is still very informal, as the focus has been on producing an easy-to-use library for raster calculations, rather than an ultra fast one. However, some of the early results, and performance tweaks, are promising and worth sharing. So, what are we comparing against? Pronto Raster is built on top of GDAL, so it cannot reasonably be expected to outperform a sensible and idiomatic way of using GDAL. Moreover, a typical application of Pronto Raster is Map Algebra. So a good benchmark case would be a cell-by-cell operation on three matrices, say OUT = 3 * A + B * C, where A, B and C are input rasters and OUT is the output raster. 

This lead to the reference code shown below, i.e. this NOT using Pronto Raster, but directly using GDAL. The code is efficient by reading in a block at the time of all the raster layers, and by reusing the memory of the previous block when the next one is read. 

```cpp
int benchmark_3_rasters_reference()
{
  GDALAllRegister();

  GDALDataset* dataset_a = (GDALDataset*)GDALOpen("random_a.tiff", GA_ReadOnly);
  GDALDataset* dataset_b = (GDALDataset*)GDALOpen("random_b.tiff", GA_ReadOnly);
  GDALDataset* dataset_c = (GDALDataset*)GDALOpen("random_c.tiff", GA_ReadOnly);
  GDALDataset* dataset_out = (GDALDataset*)GDALOpen("output.tiff", GA_Update);

  GDALRasterBand* band_a = dataset_a->GetRasterBand(1);
  GDALRasterBand* band_b = dataset_b->GetRasterBand(1);
  GDALRasterBand* band_c = dataset_c->GetRasterBand(1);
  GDALRasterBand* band_out = dataset_out->GetRasterBand(1);

  CPLAssert(poBand->GetRasterDataType() == GDT_Byte);
  int nXBlockSize, nYBlockSize;

  band_a->GetBlockSize(&nXBlockSize, &nYBlockSize);
  int nXBlocks = (band_a->GetXSize() + nXBlockSize - 1) / nXBlockSize;
  int nYBlocks = (band_a->GetYSize() + nYBlockSize - 1) / nYBlockSize;

  GByte *data_a = (GByte *)CPLMalloc(nXBlockSize * nYBlockSize);
  GByte *data_b = (GByte *)CPLMalloc(nXBlockSize * nYBlockSize);
  GByte *data_c = (GByte *)CPLMalloc(nXBlockSize * nYBlockSize);
  GByte *data_out = (GByte *)CPLMalloc(nXBlockSize * nYBlockSize);

  for (int iYBlock = 0; iYBlock < nYBlocks; iYBlock++) {
    for (int iXBlock = 0; iXBlock < nXBlocks; iXBlock++) {

      band_a->ReadBlock(iXBlock, iYBlock, data_a);
      band_b->ReadBlock(iXBlock, iYBlock, data_b);
      band_c->ReadBlock(iXBlock, iYBlock, data_c);
      int nXValid = std::min(nXBlockSize, dataset_a->GetRasterXSize() - iXBlock * nXBlockSize);
      int nYValid = std::min(nYBlockSize, dataset_a->GetRasterYSize() - iYBlock * nYBlockSize);

      // Compute the portion of the block that is valid
      // for partial edge blocks.

      for (int iY = 0; iY < nYValid; iY++) {
        for (int iX = 0; iX < nXValid; iX++) {
          int index = iX + iY * nXBlockSize;
          data_out[index] = 3 * data_a[index] + data_b[index] * data_c[index];
        }
      }
      band_out->WriteBlock(iXBlock, iYBlock, data_out);
    }
  }
  double min, max, mean, stddev;
  CPLErr try_statistics = band_out->GetStatistics(FALSE,
    FALSE,
    &min, &max, &mean, &stddev);
  // Only update statistics if rasterband thinks that they are up 
  // to date
  if (try_statistics != CE_Warning) {
    band_out->ComputeStatistics(FALSE, &min, &max, &mean, &stddev, NULL, NULL);
    band_out->SetStatistics(min, max, mean, stddev);
  }

  CPLFree(data_a);
  CPLFree(data_b);
  CPLFree(data_c);
  CPLFree(data_out);

  GDALClose(dataset_a);
  GDALClose(dataset_b);
  GDALClose(dataset_c);
  GDALClose(dataset_out);

  return 0;
}
```

The code in Pronto Raster to do the same thing is shown below. Note that here the datasets are opened using `open_any` i.e. without specifying at compile time what the data type is. As will be shown, this flexibility comes at the cost of computational performance.

```cpp
int benchmark_3_rasters_blind()
{
  auto raster_a = pr::open_any("random_a.tiff", pr::access::read_only);
  auto raster_b = pr::open_any("random_b.tiff", pr::access::read_only);
  auto raster_c = pr::open_any("random_c.tiff", pr::access::read_only);
  auto raster_out = pr::open_any("output.tiff", pr::access::read_write);

  auto raster_sum = 3 * pr::raster_algebra_wrap(raster_a) 
    + pr::raster_algebra_wrap(raster_b) * pr::raster_algebra_wrap(raster_c);

  pr::assign(raster_out, raster_sum.unwrap());

  return 0;
}
```
Alternatively if we do specify the data type at compile time(or to be more precise the data type to which cell values will be cast) then the code looks as follows:

```cpp
int benchmark_3_rasters()
{
  auto raster_a = pr::open<unsigned char>("random_a.tiff", pr::access::read_only);
  auto raster_b = pr::open<unsigned char>("random_b.tiff", pr::access::read_only);
  auto raster_c = pr::open<unsigned char>("random_c.tiff", pr::access::read_only);
  auto raster_out = pr::open<unsigned char>("output.tiff", pr::access::read_write);

 
  auto raster_sum = 3 * pr::raster_algebra_wrap(raster_a) 
    + pr::raster_algebra_wrap(raster_b) * pr::raster_algebra_wrap(raster_c);
	
  pr::assign(raster_out, raster_sum);

  return 0;
}
```

The previous solution creates raster view that allow random access iteration, which in fact comes at a performance cost. Therefore a variation was made that only supports forward iteration and can be a bit more efficient.

```cpp
int benchmark_3_rasters_forward_only()
{
  auto raster_a = pr::open_forward_only<unsigned char>("random_a.tiff", pr::access::read_only);
  auto raster_b = pr::open_forward_only<unsigned char>("random_b.tiff", pr::access::read_only);
  auto raster_c = pr::open_forward_only<unsigned char>("random_c.tiff", pr::access::read_only);
  auto raster_out = pr::open_forward_only<unsigned char>("output.tiff", pr::access::read_write);


  auto raster_sum = 3 * pr::raster_algebra_wrap(raster_a)
    + pr::raster_algebra_wrap(raster_b) * pr::raster_algebra_wrap(raster_c);

  pr::assign(raster_out, raster_sum);

  return 0;
}
```

The previous Pronto Raster based solutions iterate over the raster row-by-row and within rows column-by-column. The reference solution however has the benefit of iterating block-by-block, within each block row-by-row, and within rows column-by-column. This reduces memory usage. For this Pronto Raster has the assign_blocked function. Giving us the final Pronto Raster based benchmark.

```cpp
int benchmark_3_rasters_forward_only_in_blocks()
{
  auto raster_a = pr::open_forward_only<unsigned char>("random_a.tiff", pr::access::read_only);
  auto raster_b = pr::open_forward_only<unsigned char>("random_b.tiff", pr::access::read_only);
  auto raster_c = pr::open_forward_only<unsigned char>("random_c.tiff", pr::access::read_only);
  auto raster_out = pr::open_forward_only<unsigned char>("output.tiff", pr::access::read_write);


  auto raster_sum = 3 * pr::raster_algebra_wrap(raster_a)
    + pr::raster_algebra_wrap(raster_b) * pr::raster_algebra_wrap(raster_c);

  pr::assign_blocked(raster_out, raster_sum);

  return 0;
}
```

How do the results stack up? When applied to 1000 x 1000 rasters in 256 x 256 blocks. 

|function|time required (s)|
|----|----|
|benchmark_3_rasters_reference|0.095|
|benchmark_3_rasters_blind|0.313|
|benchmark_3_rasters|0.162|
|benchmark_3_rasters_forward_only|0.136|
|benchmark_3_rasters_forward_only_in_blocks|0.096|

It thus seems that Pronto Raster is able to approximate the performance of using GDAL directly very closely, when using all the bells and whistles (and some functions that still need to be documented). The different generalizations do come at a cost. Forward only iteration may be too limited for implementing some algorithms; iterating row-by-row is more generic than iterating column-by-column and may be preferred for some more complex operations (e.g. moving windows); and not having to specify the value type of rasters at compile time clearly is a privilege that comes at a cost.

All the functions can be found in [benchmark.cpp](https://github.com/ahhz/raster/blob/master/benchmarks/benchmark.cpp). It is obvious that this benchmarking can benefit from a more systematic treatment, e.g. using Google Benchmark as a framework. If you feel inclined to contribute, please do!

It would also be great to see comparisons with alternative scripting solutions, e.g. in R, ArcGIS Python, PC Raster Python, MATLAB, ...
