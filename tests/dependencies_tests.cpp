//
//=======================================================================
// Copyright 2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include <gtest/gtest.h>

#pragma warning( push )
#pragma warning( disable : 4251 )
#include <gdal.h>      
#include <gdal_priv.h>  
#pragma warning( pop ) 


#include <random>
#include <string>




bool test_and_close_dataset(const std::string& file, GDALAccess access)
{
	// Including this test because it often fails when linking incorrectly, 
	// e.g. Debug lib to Release exe.
	
	GDALDataset* dataset = (GDALDataset*)GDALOpen(file.c_str(), access);
	bool test = dataset->GetAccess() == access;
	GDALClose(dataset);
	return test;
}

std::string unique_path()
{

	const char hex[] = "0123456789abcdef";
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<int> dis(0, 15);

	// convert path to wstring
	std::string s = "temp_file_%%%%_%%%%_%%%%_%%%%.tif";

	// replace % for random hex number
	for (auto&& ch : s) {
		if (ch == '%') {
			ch = hex[dis(gen)];
		}
	}
	return s;
}

void create_and_close_dataset(const std::string& filename)
{
	int rows = 5;
	int cols = 5;
	int bands = 1;
	char **options = NULL;

	GDALDriverManager* m = GetGDALDriverManager();
	GDALDriver* driver = m->GetDriverByName("GTiff");
	GDALDataset* dataset = driver->Create(filename.c_str(), cols, rows
		, bands, GDT_Int16, options);
	
	GDALClose(dataset);
}

void delete_dataset(const std::string& file)
{
	GDALDataset* dataset = (GDALDataset*)GDALOpen(file.c_str(), GA_ReadOnly);
	char** file_list = dataset->GetFileList();
	GDALDriver* driver = dataset->GetDriver();
	GDALClose(dataset);
	driver->Delete(file_list[0]);
	CSLDestroy(file_list);
}
bool test_gdal_access_read_only()
{
	GDALAllRegister(); 

	std::string path = unique_path();
	create_and_close_dataset(path);
	bool test_result = test_and_close_dataset(path, GA_ReadOnly);
	delete_dataset(path);

	return test_result;
}

bool test_gdal_access_update()
{
	GDALAllRegister();

	std::string path = unique_path();
	create_and_close_dataset(path);
	bool test_result = test_and_close_dataset(path, GA_Update);
	delete_dataset(path);

	return test_result;
}


TEST(RasterTest, Dependencies) {
  EXPECT_TRUE(test_gdal_access_read_only());
  EXPECT_TRUE(test_gdal_access_update());
}
