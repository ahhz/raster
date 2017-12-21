


#include <blink/raster/any_blind_raster.h>
#include <blink/raster/assign.h>
#include <blink/raster/raster_algebra.h>
#include <blink/raster/raster_algebra_operators.h>


#include <blink/raster/io.h>
#include <blink/raster/random_raster_view.h>
#include <blink/raster/uniform_raster_view.h>


#include <chrono>
#include <iostream>
#include <random>



namespace br = blink::raster;

void create_data_for_benchmark(int rows, int cols)
{
	auto raster_a = br::create<int>("random_a.tiff", rows, cols, GDT_Byte);
	auto raster_b = br::create<int>("random_b.tiff", rows, cols, GDT_Byte);
	auto raster_c = br::create<int>("random_c.tiff", rows, cols, GDT_Byte);
	auto raster_d = br::create<int>("output.tiff", rows, cols, GDT_Byte);
	
	
	 // Choose a distribution to use, here the fair dice distribution
  auto dist = std::uniform_int_distribution<int>(1, 6);
  
  // Choose a generator to use, and instantiate the generator with a seed
  auto gen_a = std::mt19937( std::random_device{}() );
  auto gen_b = std::mt19937( std::random_device{}() );
  auto gen_c = std::mt19937( std::random_device{}() );

  // Create the random raster
 auto random_a = br::random_distribution_raster(rows, cols, dist, gen_a);
 auto random_b = br::random_distribution_raster(rows, cols, dist, gen_b);
 auto random_c = br::random_distribution_raster(rows, cols, dist, gen_c);

  br::assign(raster_a, random_a);
  br::assign(raster_b, random_b);
  br::assign(raster_c, random_c);
 }

int benchmark_3_rasters()
{
  auto raster_a = br::open<int>("random_a.tiff", br::access::read_only);
  auto raster_b = br::open<int>("random_b.tiff", br::access::read_only);
  auto raster_c = br::open<int>("random_c.tiff", br::access::read_only);
  auto raster_out = br::open<int>("output.tiff", br::access::read_write);

 
  auto raster_sum = 3 * br::raster_algebra_wrap(raster_a) 
    + br::raster_algebra_wrap(raster_b) * br::raster_algebra_wrap(raster_c);
	
  br::assign(raster_out, raster_sum);

  return 0;
}

int benchmark_3_rasters_blind()
{
  auto raster_a = br::open_any("random_a.tiff", br::access::read_only);
  auto raster_b = br::open_any("random_b.tiff", br::access::read_only);
  auto raster_c = br::open_any("random_c.tiff", br::access::read_only);
  auto raster_out = br::open_any("output.tiff", br::access::read_write);

  auto raster_sum = 3 * br::raster_algebra_wrap(raster_a) 
    + br::raster_algebra_wrap(raster_b) * br::raster_algebra_wrap(raster_c);

  br::assign(raster_out, raster_sum.unwrap());

  return 0;
}

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
  std::cout << " Check raster type: " << (band_a->GetRasterDataType() == GDT_Byte) << std::endl;
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


int benchmark_2_rasters()
{
  auto raster_a = br::open<int>("random_a.tiff", br::access::read_only);
  auto raster_b = br::open<int>("random_b.tiff", br::access::read_only);
  auto raster_out = br::open<int>("output.tiff", br::access::read_write);


  auto raster_sum = 3 * br::raster_algebra_wrap(raster_a)
    + br::raster_algebra_wrap(raster_b);

  //br::assign_blocked(raster_out, raster_sum,256, 256);
  br::assign(raster_out, raster_sum);

  return 0;
}

int benchmark_2_rasters_blind()
{
  auto raster_a = br::open_any("random_a.tiff", br::access::read_only);
  auto raster_b = br::open_any("random_b.tiff", br::access::read_only);
  auto raster_out = br::open_any("output.tiff", br::access::read_write);

  auto raster_sum = 3 * br::raster_algebra_wrap(raster_a)
    + br::raster_algebra_wrap(raster_b);

  br::assign(raster_out, raster_sum.unwrap());

  return 0;
}

int benchmark_2_rasters_reference()
{
  GDALAllRegister();
  GDALDataset* dataset_a = (GDALDataset*)GDALOpen("random_a.tiff", GA_ReadOnly);
  GDALDataset* dataset_b = (GDALDataset*)GDALOpen("random_b.tiff", GA_ReadOnly);
  GDALDataset* dataset_out = (GDALDataset*)GDALOpen("output.tiff", GA_Update);

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

  return 0;
}

int benchmark_assign()
{
  auto raster_a = br::open<int>("random_a.tiff", br::access::read_only);
  auto raster_out = br::open<int>("output.tiff", br::access::read_write);
  br::assign(raster_out, raster_a);
  return 0;
}

int benchmark_assign_blind()
{

  // Some code  

  auto raster_a = br::open_any("random_a.tiff", br::access::read_only);
  auto raster_out = br::open_any("output.tiff", br::access::read_write);
  br::assign(raster_out, raster_a);
  return 0;
}

int benchmark_assign_reference()
{
  GDALAllRegister();
  GDALDataset* dataset_a = (GDALDataset*)GDALOpen("random_a.tiff", GA_ReadOnly);
  GDALDataset* dataset_out = (GDALDataset*)GDALOpen("output.tiff", GA_Update);

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



int main()
{
  auto start = std::chrono::system_clock::now();
  
  //create_data_for_benchmark(1000, 1000);
  //benchmark_2_rasters();
  // benchmark_2_rasters_blind();
  // benchmark_2_rasters_reference();
  
  //benchmark_3_rasters();
  // benchmark_3rasters_blind();
  //benchmark_3_rasters_reference();

  benchmark_assign();
  //benchmark_assign_blind();
  //benchmark_assign_reference();


  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = end - start;
  std::cout << "That took " << diff.count() << " seconds" << std::endl;
  return 0;
}