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

#include <pronto/raster/io.h>
#include <pronto/raster/gdal_raster_view.h>

#include <vector>

namespace pr = pronto::raster;
namespace fs = pr::filesystem;

bool test_assign_reference_proxy()
{
	auto r = pr::create_temp<double>(1, 1);
	auto iter = r.begin();
	*iter = 3.0;
	return *iter == 3;
}

bool test_increment_reference_proxy()
{
	auto r = pr::create_temp<double>(1, 1);
	auto iter = r.begin();
	*iter = 3.0;
	(*iter) += 2.0;
	return *iter == 5.0;
}

TEST(RasterTest, ReferenceProxy) {
  EXPECT_TRUE(test_assign_reference_proxy());
  EXPECT_TRUE(test_increment_reference_proxy());
}
