//
//=======================================================================
// Copyright 2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#include <pronto/raster/any_blind_raster.h>
#include <pronto/raster/assign.h>
#include <pronto/raster/io.h>
#include <pronto/raster/random_raster_view.h>
#include <pronto/raster/raster_algebra_operators.h>
#include <pronto/raster/raster_algebra_wrapper.h>
#include <pronto/raster/filesystem.h>

#include <benchmark/benchmark.h>

#include <random>
#include <iostream>

namespace pr = pronto::raster;

void create_data_for_benchmark(int rows, int cols)
{
	auto raster_a = pr::create<int>("random_a.tif", rows, cols, GDT_Byte);
	auto raster_b = pr::create<int>("random_b.tif", rows, cols, GDT_Byte);
	auto raster_c = pr::create<int>("random_c.tif", rows, cols, GDT_Byte);
	auto raster_out = pr::create<int>("output.tif", rows, cols, GDT_Byte);
	
  {
    // Choose a distribution to use, here the fair dice distribution
    auto dist = std::uniform_int_distribution<int>(1, 6);

    // Choose a generator to use, and instantiate the generator with a seed
    auto gen_a = std::mt19937(std::random_device{}());
    auto gen_b = std::mt19937(std::random_device{}());
    auto gen_c = std::mt19937(std::random_device{}());

    // Create the random raster
    auto random_a = pr::random_distribution_raster(rows, cols, dist, gen_a);
    auto random_b = pr::random_distribution_raster(rows, cols, dist, gen_b);
    auto random_c = pr::random_distribution_raster(rows, cols, dist, gen_c);
    auto zeros = pr::uniform(rows, cols, static_cast<unsigned char>(0));
    pr::assign(raster_a, random_a);
    pr::assign(raster_b, random_b);
    pr::assign(raster_c, random_c);
    pr::assign(raster_out, zeros);
  }
  std::cout << "Files size of output.tif: " << pr::filesystem::file_size("output.tif") << std::endl;

}

static void BM_create_data_for_benchmark(benchmark::State& state) {
  for (auto _ : state)
    create_data_for_benchmark(10000, 10000);
}

int benchmark_3_rasters_pronto()
{
  {
    auto raster_a = pr::open<unsigned char>("random_a.tif", pr::access::read_only);
    auto raster_b = pr::open<unsigned char>("random_b.tif", pr::access::read_only);
    auto raster_c = pr::open<unsigned char>("random_c.tif", pr::access::read_only);
    auto raster_out = pr::open<unsigned char>("output.tif", pr::access::read_write);


    auto raster_sum = 3 * pr::raster_algebra_wrap(raster_a)
      + pr::raster_algebra_wrap(raster_b) * pr::raster_algebra_wrap(raster_c);

    pr::assign(raster_out, raster_sum);
  }
  std::cout << "Files size of output.tif: " << pr::filesystem::file_size("output.tif") << std::endl;


  return 0;
}

static void BM_3_rasters_pronto(benchmark::State& state) {
  for (auto _ : state)
    benchmark_3_rasters_pronto();
}

int benchmark_3_rasters_pronto_forward_only()
{
  {
    auto raster_a = pr::open_forward_only<unsigned char>("random_a.tif", pr::access::read_only);
    auto raster_b = pr::open_forward_only<unsigned char>("random_b.tif", pr::access::read_only);
    auto raster_c = pr::open_forward_only<unsigned char>("random_c.tif", pr::access::read_only);
    auto raster_out = pr::open_forward_only<unsigned char>("output.tif", pr::access::read_write);


    auto raster_sum = 3 * pr::raster_algebra_wrap(raster_a)
      + pr::raster_algebra_wrap(raster_b) * pr::raster_algebra_wrap(raster_c);

    pr::assign(raster_out, raster_sum);
  }
  std::cout << "Files size of output.tif: " << pr::filesystem::file_size("output.tif") << std::endl;

  return 0;
}

static void BM_3_rasters_pronto_forward_only(benchmark::State& state) {
  for (auto _ : state)
    benchmark_3_rasters_pronto_forward_only();
}

int benchmark_3_rasters_pronto_blind()
{
  {
    auto raster_a = pr::open_any("random_a.tif", pr::access::read_only);
    auto raster_b = pr::open_any("random_b.tif", pr::access::read_only);
    auto raster_c = pr::open_any("random_c.tif", pr::access::read_only);
    auto raster_out = pr::open_any("output.tif", pr::access::read_write);

    auto raster_sum = 3 * pr::raster_algebra_wrap(raster_a)
      + pr::raster_algebra_wrap(raster_b) * pr::raster_algebra_wrap(raster_c);

    pr::assign(raster_out, raster_sum.unwrap());
  }
  std::cout << "Files size of output.tif: " << pr::filesystem::file_size("output.tif") << std::endl;


  return 0;
}
static void BM_3_rasters_pronto_blind(benchmark::State& state) {
  for (auto _ : state)
    benchmark_3_rasters_pronto_blind();
}

int benchmark_3_rasters_pronto_forward_only_in_blocks()
{
  {
    auto raster_a = pr::open_forward_only<unsigned char>("random_a.tif", pr::access::read_only);
    auto raster_b = pr::open_forward_only<unsigned char>("random_b.tif", pr::access::read_only);
    auto raster_c = pr::open_forward_only<unsigned char>("random_c.tif", pr::access::read_only);
    auto raster_out = pr::open_forward_only<unsigned char>("output.tif", pr::access::read_write);


    auto raster_sum = 3 * pr::raster_algebra_wrap(raster_a)
      + pr::raster_algebra_wrap(raster_b) * pr::raster_algebra_wrap(raster_c);

    pr::assign_blocked(raster_out, raster_sum);
  }
   std::cout << "Files size of output.tif: " << pr::filesystem::file_size("output.tif") << std::endl;

  return 0;
}

static void BM_3_rasters_pronto_forward_only_in_blocks(benchmark::State& state) {
  for (auto _ : state)
    benchmark_3_rasters_pronto_forward_only_in_blocks();
}

int benchmark_3_rasters_pronto_forward_only_in_blocks_transform()
{
	{
		auto raster_a = pr::open_forward_only<unsigned char>("random_a.tif", pr::access::read_only);
		auto raster_b = pr::open_forward_only<unsigned char>("random_b.tif", pr::access::read_only);
		auto raster_c = pr::open_forward_only<unsigned char>("random_c.tif", pr::access::read_only);
		auto raster_out = pr::open_forward_only<unsigned char>("output.tif", pr::access::read_write);

		auto func = [](const unsigned char& a, const unsigned char& b, const unsigned char& c)
		{
			return static_cast<unsigned char>(3 * a + b * c);
		};

		auto raster_sum = pr::transform(func, raster_a, raster_b, raster_c);
		pr::assign_blocked(raster_out, raster_sum);
	}
  std::cout << "Files size of output.tif: " << pr::filesystem::file_size("output.tif") << std::endl;
  return 0;
}

static void BM_3_rasters_pronto_forward_only_in_blocks_transform(benchmark::State& state) {
  for (auto _ : state)
    benchmark_3_rasters_pronto_forward_only_in_blocks_transform();
}

int benchmark_3_rasters_pronto_forward_only_no_blocks_iterate()
{
	{
		auto raster_a = pr::open_forward_only<unsigned char>("random_a.tif", pr::access::read_only);
		auto raster_b = pr::open_forward_only<unsigned char>("random_b.tif", pr::access::read_only);
		auto raster_c = pr::open_forward_only<unsigned char>("random_c.tif", pr::access::read_only);
		auto raster_out = pr::open_forward_only<unsigned char>("output.tif", pr::access::read_write);

		auto ia = std::as_const(raster_a).begin();
		auto ib = std::as_const(raster_b).begin();
		auto ic = std::as_const(raster_c).begin();
		auto iout = raster_out.begin();

		const auto ia_end = std::as_const(raster_a).end();
		for (; ia != ia_end; ++ia, ++ib, ++ic, ++iout)
		{
			*iout = 3 * (*ia) + (*ib) * (*ic);
		}
	}
	std::cout << "Files size of output.tif: " << pr::filesystem::file_size("output.tif") << std::endl;

  
  return 0;
}
static void BM_3_rasters_pronto_forward_only_no_blocks_iterate(benchmark::State& state) {
  for (auto _ : state)
    benchmark_3_rasters_pronto_forward_only_no_blocks_iterate();
}

int benchmark_3_rasters_reference_readblock()
{
  GDALAllRegister();
  GDALDataset* dataset_a = (GDALDataset*)GDALOpen("random_a.tif", GA_ReadOnly);
  GDALDataset* dataset_b = (GDALDataset*)GDALOpen("random_b.tif", GA_ReadOnly);
  GDALDataset* dataset_c = (GDALDataset*)GDALOpen("random_c.tif", GA_ReadOnly);
  GDALDataset* dataset_out = (GDALDataset*)GDALOpen("output.tif", GA_Update);

  GDALRasterBand* band_a = dataset_a->GetRasterBand(1);
  GDALRasterBand* band_b = dataset_b->GetRasterBand(1);
  GDALRasterBand* band_c = dataset_c->GetRasterBand(1);
  GDALRasterBand* band_out = dataset_out->GetRasterBand(1);


  CPLAssert(band_a->GetRasterDataType() == GDT_Byte);
  CPLAssert(band_b->GetRasterDataType() == GDT_Byte);
  CPLAssert(band_c->GetRasterDataType() == GDT_Byte);
  CPLAssert(band_out->GetRasterDataType() == GDT_Byte);
  //std::cout << " Check raster type: " << (band_a->GetRasterDataType() == GDT_Byte) << std::endl;
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

  std::cout << "Files size of output.tif: " << pr::filesystem::file_size("output.tif") << std::endl;

  return 0;
}

static void BM_3_rasters_reference_readblock(benchmark::State& state) {
  for (auto _ : state)
    benchmark_3_rasters_reference_readblock();
}

int benchmark_3_rasters_reference_rasterio()
{
  GDALAllRegister();
  GDALDataset* dataset_a = (GDALDataset*)GDALOpen("random_a.tif", GA_ReadOnly);
  GDALDataset* dataset_b = (GDALDataset*)GDALOpen("random_b.tif", GA_ReadOnly);
  GDALDataset* dataset_c = (GDALDataset*)GDALOpen("random_c.tif", GA_ReadOnly);
  GDALDataset* dataset_out = (GDALDataset*)GDALOpen("output.tif", GA_Update);

  GDALRasterBand* band_a = dataset_a->GetRasterBand(1);
  GDALRasterBand* band_b = dataset_b->GetRasterBand(1);
  GDALRasterBand* band_c = dataset_c->GetRasterBand(1);
  GDALRasterBand* band_out = dataset_out->GetRasterBand(1);


  CPLAssert(band_a->GetRasterDataType() == GDT_Byte);
  CPLAssert(band_b->GetRasterDataType() == GDT_Byte);
  CPLAssert(band_c->GetRasterDataType() == GDT_Byte);
  CPLAssert(band_out->GetRasterDataType() == GDT_Byte);
  //std::cout << " Check raster type: " << (band_a->GetRasterDataType() == GDT_Byte) << std::endl;
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

      // Compute the portion of the block that is valid
      // for partial edge blocks.

      int nXValid = std::min(nXBlockSize, dataset_a->GetRasterXSize() - iXBlock * nXBlockSize);
      int nYValid = std::min(nYBlockSize, dataset_a->GetRasterYSize() - iYBlock * nYBlockSize);

      band_a->RasterIO(GF_Read, iXBlock * nXBlockSize, iYBlock * nYBlockSize, nXValid, nYValid, data_a, nXBlockSize, nYBlockSize, GDT_Byte, 0, 0);
      band_b->RasterIO(GF_Read, iXBlock * nXBlockSize, iYBlock * nYBlockSize, nXValid, nYValid, data_b, nXBlockSize, nYBlockSize, GDT_Byte, 0, 0);
      band_c->RasterIO(GF_Read, iXBlock * nXBlockSize, iYBlock * nYBlockSize, nXValid, nYValid, data_b, nXBlockSize, nYBlockSize, GDT_Byte, 0, 0);

      for (int iY = 0; iY < nYValid; iY++) {
        for (int iX = 0; iX < nXValid; iX++) {
          int index = iX + iY * nXBlockSize;
          data_out[index] = 3 * data_a[index] + data_b[index] * data_c[index];
        }
      }
      band_out->RasterIO(GF_Write, iXBlock * nXBlockSize, iYBlock * nYBlockSize, nXValid, nYValid, data_out, nXBlockSize, nYBlockSize, GDT_Byte, 0, 0);
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

  std::cout << "Files size of output.tif: " << pr::filesystem::file_size("output.tif") <<std::endl;

  return 0;
}

static void BM_3_rasters_reference_rasterio(benchmark::State& state) {
  for (auto _ : state)
    benchmark_3_rasters_reference_rasterio();
}



int benchmark_3_rasters_reference_getlockedblockref()
{
  GDALAllRegister();
  GDALDataset* dataset_a = (GDALDataset*)GDALOpen("random_a.tif", GA_ReadOnly);
  GDALDataset* dataset_b = (GDALDataset*)GDALOpen("random_b.tif", GA_ReadOnly);
  GDALDataset* dataset_c = (GDALDataset*)GDALOpen("random_c.tif", GA_ReadOnly);
  GDALDataset* dataset_out = (GDALDataset*)GDALOpen("output.tif", GA_Update);

  GDALRasterBand* band_a = dataset_a->GetRasterBand(1);
  GDALRasterBand* band_b = dataset_b->GetRasterBand(1);
  GDALRasterBand* band_c = dataset_c->GetRasterBand(1);
  GDALRasterBand* band_out = dataset_out->GetRasterBand(1);


  CPLAssert(band_a->GetRasterDataType() == GDT_Byte);
  CPLAssert(band_b->GetRasterDataType() == GDT_Byte);
  CPLAssert(band_c->GetRasterDataType() == GDT_Byte);
  CPLAssert(band_out->GetRasterDataType() == GDT_Byte);
  //std::cout << " Check raster type: " << (band_a->GetRasterDataType() == GDT_Byte) << std::endl;
  int nXBlockSize, nYBlockSize;

  band_a->GetBlockSize(&nXBlockSize, &nYBlockSize);
  int nXBlocks = (band_a->GetXSize() + nXBlockSize - 1) / nXBlockSize;
  int nYBlocks = (band_a->GetYSize() + nYBlockSize - 1) / nYBlockSize;



  for (int iYBlock = 0; iYBlock < nYBlocks; iYBlock++) {
    for (int iXBlock = 0; iXBlock < nXBlocks; iXBlock++) {
      GDALRasterBlock* block_a = band_a->GetLockedBlockRef(iXBlock, iYBlock);
      GDALRasterBlock* block_b = band_b->GetLockedBlockRef(iXBlock, iYBlock);
      GDALRasterBlock* block_c = band_c->GetLockedBlockRef(iXBlock, iYBlock);
      GDALRasterBlock* block_out = band_out->GetLockedBlockRef(iXBlock, iYBlock);

      GByte *data_a = (GByte *)block_a->GetDataRef();
      GByte *data_b = (GByte *)block_b->GetDataRef();
      GByte *data_c = (GByte *)block_c->GetDataRef();
      GByte *data_out = (GByte *)block_out->GetDataRef();


      // Compute the portion of the block that is valid
      // for partial edge blocks.

      int nXValid = std::min(nXBlockSize, dataset_a->GetRasterXSize() - iXBlock * nXBlockSize);
      int nYValid = std::min(nYBlockSize, dataset_a->GetRasterYSize() - iYBlock * nYBlockSize);
      int stride = GDALGetDataTypeSize(GDT_Byte) / 8;

   
      for (int iY = 0; iY < nYValid; iY++) {
        for (int iX = 0; iX < nXValid; iX++) {
          int index = iX + iY * nXBlockSize;
          data_out[index] = 3 * data_a[index] + data_b[index] * data_c[index];
        }
      }
      block_out->MarkDirty();
      block_out->Write();
      block_a->DropLock();
      block_b->DropLock();
      block_c->DropLock();
      block_out->DropLock();
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


  GDALClose(dataset_a);
  GDALClose(dataset_b);
  GDALClose(dataset_c);
  GDALClose(dataset_out);
  std::cout << "Files size of output.tif: " << pr::filesystem::file_size("output.tif") << std::endl;

  return 0;
}

static void BM_3_rasters_reference_getlockedblockref(benchmark::State& state) {
  for (auto _ : state)
    benchmark_3_rasters_reference_getlockedblockref();
}

int benchmark_2_rasters()
{
  {
    auto raster_a = pr::open<int>("random_a.tif", pr::access::read_only);
    auto raster_b = pr::open<int>("random_b.tif", pr::access::read_only);
    auto raster_out = pr::open<int>("output.tif", pr::access::read_write);


    auto raster_sum = 3 * pr::raster_algebra_wrap(raster_a)
      + pr::raster_algebra_wrap(raster_b);

    //pr::assign_blocked(raster_out, raster_sum,256, 256);
    pr::assign(raster_out, raster_sum);
  }
  std::cout << "Files size of output.tif: " << pr::filesystem::file_size("output.tif") << std::endl;

  return 0;
}

int benchmark_2_rasters_blind()
{
  {
    auto raster_a = pr::open_any("random_a.tif", pr::access::read_only);
    auto raster_b = pr::open_any("random_b.tif", pr::access::read_only);
    auto raster_out = pr::open_any("output.tif", pr::access::read_write);

    auto raster_sum = 3 * pr::raster_algebra_wrap(raster_a)
      + pr::raster_algebra_wrap(raster_b);

    pr::assign(raster_out, raster_sum.unwrap());
  }
  std::cout << "Files size of output.tif: " << pr::filesystem::file_size("output.tif") << std::endl;

  return 0;
}

int benchmark_2_rasters_reference()
{
  GDALAllRegister();
  GDALDataset* dataset_a = (GDALDataset*)GDALOpen("random_a.tif", GA_ReadOnly);
  GDALDataset* dataset_b = (GDALDataset*)GDALOpen("random_b.tif", GA_ReadOnly);
  GDALDataset* dataset_out = (GDALDataset*)GDALOpen("output.tif", GA_Update);

  GDALRasterBand* band_a = dataset_a->GetRasterBand(1);
  GDALRasterBand* band_b = dataset_b->GetRasterBand(1);
  GDALRasterBand* band_out = dataset_out->GetRasterBand(1);


  CPLAssert(poBand->GetRasterDataType() == GDT_Byte);
  int nXBlockSize, nYBlockSize;

  band_a->GetBlockSize(&nXBlockSize, &nYBlockSize);
  int nXBlocks = (band_a->GetXSize() + nXBlockSize - 1) / nXBlockSize;
  int nYBlocks = (band_a->GetYSize() + nYBlockSize - 1) / nYBlockSize;

  GByte *data_a = (GByte *)CPLMalloc(nXBlockSize * nYBlockSize);
  GByte *data_b = (GByte *)CPLMalloc(nXBlockSize * nYBlockSize);
  GByte *data_out = (GByte *)CPLMalloc(nXBlockSize * nYBlockSize);

  for (int iYBlock = 0; iYBlock < nYBlocks; iYBlock++) {
    for (int iXBlock = 0; iXBlock < nXBlocks; iXBlock++) {

      band_a->ReadBlock(iXBlock, iYBlock, data_a);
      band_b->ReadBlock(iXBlock, iYBlock, data_b);
      int nXValid = std::min(nXBlockSize, dataset_a->GetRasterXSize() - iXBlock * nXBlockSize);
      int nYValid = std::min(nYBlockSize, dataset_a->GetRasterYSize() - iYBlock * nYBlockSize);

      // Compute the portion of the block that is valid
      // for partial edge blocks.

      for (int iY = 0; iY < nYValid; iY++) {
        for (int iX = 0; iX < nXValid; iX++) {
          int index = iX + iY * nXBlockSize;
          data_out[index] = 3 * data_a[index] + data_b[index] ;
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
  CPLFree(data_out);

  GDALClose(dataset_a);
  GDALClose(dataset_b);
  GDALClose(dataset_out);
  std::cout << "Files size of output.tif: " << pr::filesystem::file_size("output.tif") << std::endl;

  return 0;
}



CPLErr TestABCFunction(void **papoSources, int nSources, void *pData,
  int nXSize, int nYSize,
  GDALDataType eSrcType, GDALDataType eBufType,
  int nPixelSpace, int nLineSpace)
{
  int ii, iLine, iCol;
  unsigned char pix_val;
  double a, b, c;
  // ---- Init ----
  if (nSources != 3) return CE_Failure;
  // ---- Set pixels ----
  for (iLine = 0; iLine < nYSize; iLine++)
  {
    for (iCol = 0; iCol < nXSize; iCol++)
    {
      ii = iLine * nXSize + iCol;
      /* Source raster pixels may be obtained with SRCVAL macro */
      a = SRCVAL(papoSources[0], eSrcType, ii);
      b = SRCVAL(papoSources[1], eSrcType, ii);
      c = SRCVAL(papoSources[2], eSrcType, ii);
      pix_val = static_cast<unsigned char>(3 * a + b * c);
      GDALCopyWords(&pix_val, GDT_Byte, 0,
        ((GByte *)pData) + nLineSpace * iLine + iCol * nPixelSpace,
        eBufType, nPixelSpace, 1);
    }
  }
  // ---- Return success ----
  return CE_None;
}

int benchmark_3_rasters_reference_pixel_function()
{
  GDALAllRegister();
  GDALAddDerivedBandPixelFunc("MyABCFunction", TestABCFunction);

  GDALDataset* dataset_abc = (GDALDataset*)GDALOpen("abc.vrt", GA_ReadOnly);
  GDALDataset* dataset_out = (GDALDataset*)GDALOpen("output.tif", GA_Update);

  GDALRasterBand* band_abc = dataset_abc->GetRasterBand(1);
  GDALRasterBand* band_out = dataset_out->GetRasterBand(1);

  CPLAssert(band_abc->GetRasterDataType() == GDT_Byte);
  int nXBlockSize, nYBlockSize;

  band_out->GetBlockSize(&nXBlockSize, &nYBlockSize);
  int nXBlocks = (band_abc->GetXSize() + nXBlockSize - 1) / nXBlockSize;
  int nYBlocks = (band_abc->GetYSize() + nYBlockSize - 1) / nYBlockSize;

  GByte *data_abc = (GByte *)CPLMalloc(nXBlockSize * nYBlockSize);
  
  for (int iYBlock = 0; iYBlock < nYBlocks; iYBlock++) {
    for (int iXBlock = 0; iXBlock < nXBlocks; iXBlock++) {

      int nXValid = std::min(nXBlockSize, dataset_abc->GetRasterXSize() - 
        iXBlock * nXBlockSize);
      int nYValid = std::min(nYBlockSize, dataset_abc->GetRasterYSize() - 
        iYBlock * nYBlockSize);

      band_abc->RasterIO(GF_Read, iXBlock * nXBlockSize, iYBlock * nYBlockSize
        , nXValid, nYValid, data_abc, nXBlockSize, nYBlockSize, GDT_Byte, 0, 0);
      band_out->WriteBlock(iXBlock, iYBlock, data_abc);
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

  CPLFree(data_abc);
  GDALClose(dataset_out);

  std::cout << "Files size of output.tif: " << pr::filesystem::file_size("output.tif") << std::endl;


  return 0;
}

static void BM_3_rasters_reference_pixel_function(benchmark::State& state) {
  for (auto _ : state)
    benchmark_3_rasters_reference_pixel_function();
}

int benchmark_3_rasters_reference_python_pixel_function()
{
  GDALAllRegister();
  CPLSetConfigOption("GDAL_VRT_ENABLE_PYTHON", "YES");
  GDALDataset* dataset_abc = (GDALDataset*)GDALOpen("abc_python.vrt", GA_ReadOnly);
  GDALDataset* dataset_out = (GDALDataset*)GDALOpen("output.tif", GA_Update);

  GDALRasterBand* band_abc = dataset_abc->GetRasterBand(1);
  GDALRasterBand* band_out = dataset_out->GetRasterBand(1);

  CPLAssert(band_abc->GetRasterDataType() == GDT_Byte);
  int nXBlockSize, nYBlockSize;

  band_out->GetBlockSize(&nXBlockSize, &nYBlockSize);
  int nXBlocks = (band_abc->GetXSize() + nXBlockSize - 1) / nXBlockSize;
  int nYBlocks = (band_abc->GetYSize() + nYBlockSize - 1) / nYBlockSize;

  GByte *data_abc = (GByte *)CPLMalloc(nXBlockSize * nYBlockSize);

  for (int iYBlock = 0; iYBlock < nYBlocks; iYBlock++) {
    for (int iXBlock = 0; iXBlock < nXBlocks; iXBlock++) {

      int nXValid = std::min(nXBlockSize, dataset_abc->GetRasterXSize() -
        iXBlock * nXBlockSize);
      int nYValid = std::min(nYBlockSize, dataset_abc->GetRasterYSize() -
        iYBlock * nYBlockSize);

      band_abc->RasterIO(GF_Read, iXBlock * nXBlockSize, iYBlock * nYBlockSize
        , nXValid, nYValid, data_abc, nXBlockSize, nYBlockSize, GDT_Byte, 0, 0);
      band_out->WriteBlock(iXBlock, iYBlock, data_abc);
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

  CPLFree(data_abc);
  GDALClose(dataset_out);
  std::cout << "Files size of output.tif: " << pr::filesystem::file_size("output.tif") << std::endl;

  return 0;
}

static void BM_3_rasters_reference_python_pixel_function(benchmark::State& state) {
  for (auto _ : state)
    benchmark_3_rasters_reference_python_pixel_function();
}
static void BM_3_rasters_coldstart(benchmark::State& state) {
	for (auto _ : state)
		benchmark_3_rasters_reference_readblock();
}
BENCHMARK(BM_create_data_for_benchmark)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_3_rasters_coldstart)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_3_rasters_pronto_blind)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_3_rasters_pronto)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_3_rasters_pronto_forward_only)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_3_rasters_pronto_forward_only_no_blocks_iterate)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_3_rasters_pronto_forward_only_in_blocks)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_3_rasters_pronto_forward_only_in_blocks_transform)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_3_rasters_reference_readblock)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_3_rasters_reference_rasterio)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_3_rasters_reference_getlockedblockref)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_3_rasters_reference_pixel_function)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_3_rasters_reference_python_pixel_function)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();

int benchmark_assign()
{
  auto raster_a = pr::open<int>("random_a.tif", pr::access::read_only);
  auto raster_out = pr::open<int>("output.tif", pr::access::read_write);
  pr::assign(raster_out, raster_a);
  return 0;
}

int benchmark_assign_blind()
{
  auto raster_a = pr::open_any("random_a.tif", pr::access::read_only);
  auto raster_out = pr::open_any("output.tif", pr::access::read_write);
  pr::assign(raster_out, raster_a);
  return 0;
}

int benchmark_assign_reference()
{
  GDALAllRegister();
  GDALDataset* dataset_a = (GDALDataset*)GDALOpen("random_a.tif", GA_ReadOnly);
  GDALDataset* dataset_out = (GDALDataset*)GDALOpen("output.tif", GA_Update);

  GDALRasterBand* band_a = dataset_a->GetRasterBand(1);
  GDALRasterBand* band_out = dataset_out->GetRasterBand(1);


  CPLAssert(poBand->GetRasterDataType() == GDT_Byte);
  std::cout << " Check raster type: " << (band_a->GetRasterDataType() == GDT_Byte) << std::endl;
  int nXBlockSize, nYBlockSize;

  band_a->GetBlockSize(&nXBlockSize, &nYBlockSize);
  int nXBlocks = (band_a->GetXSize() + nXBlockSize - 1) / nXBlockSize;
  int nYBlocks = (band_a->GetYSize() + nYBlockSize - 1) / nYBlockSize;

  GByte *data_a = (GByte *)CPLMalloc(nXBlockSize * nYBlockSize);
  GByte *data_out = (GByte *)CPLMalloc(nXBlockSize * nYBlockSize);

  for (int iYBlock = 0; iYBlock < nYBlocks; iYBlock++) {
    for (int iXBlock = 0; iXBlock < nXBlocks; iXBlock++) {

      band_a->ReadBlock(iXBlock, iYBlock, data_a);
      int nXValid = std::min(nXBlockSize, dataset_a->GetRasterXSize() - iXBlock * nXBlockSize);
      int nYValid = std::min(nYBlockSize, dataset_a->GetRasterYSize() - iYBlock * nYBlockSize);

      // Compute the portion of the block that is valid
      // for partial edge blocks.

      for (int iY = 0; iY < nYValid; iY++) {
        for (int iX = 0; iX < nXValid; iX++) {
          int index = iX + iY * nXBlockSize;
          data_out[index] = data_a[index];
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
  CPLFree(data_out);

  GDALClose(dataset_a);
  GDALClose(dataset_out);

  return 0;
}