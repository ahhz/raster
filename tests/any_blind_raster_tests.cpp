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

#include <pronto/raster/any_blind_raster.h>
#include <pronto/raster/blind_function.h>
#include <pronto/raster/io.h>


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
	
	auto index = pr::detail::index_in_packed_list(blind_a, pr::blind_data_types{});
	return index == 4;
}


TEST(RasterTest, AnyBlindRaster) {
  EXPECT_TRUE(test_get_any_blind_raster());
}

