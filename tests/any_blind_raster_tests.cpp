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
#include <pronto/raster/raster_algebra_wrapper.h>
#include <pronto/raster/raster_algebra_operators.h>

#include <vector>

namespace pr = pronto::raster;


bool test_get_any_blind_raster()
{
	int rows = 3;
	int cols = 5;
	auto a = pr::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	pr::any_raster<int> any_a(a);
	pr::any_blind_raster blind_a(any_a);
	
	int index = blind_a.index();
	return index == 4;
}


TEST(RasterTest, AnyBlindRaster) {
  EXPECT_TRUE(test_get_any_blind_raster());
}

