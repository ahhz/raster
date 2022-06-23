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
#include <pronto/raster/raster_variant.h>
#include <pronto/raster/raster_algebra_operators.h>

#include <vector>

namespace pr = pronto::raster;
namespace fs = std::filesystem;

bool test_raster_plus_raster()
{
	int rows = 3;
	int cols = 5;
	auto a = pr::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	auto b = pr::create_temp<int>(rows, cols);
	int hundreds = 0;
	for (auto&& i : b) {
		hundreds += 100;
		i = hundreds;
	}

	auto c = a+b;
	int check = 0;
	for (auto&& i : c) {
		check += 101;
		if (i != check) {
			return false;
		}
	}
	return check == rows * cols * 101;
}

bool test_raster_mod_raster()
{
	int rows = 3;
	int cols = 5;
	auto a = pr::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	auto b = pr::create_temp<int>(rows, cols);
	int b_val = 7;
	for (auto&& i : b) {
		b_val += 77;
		i = b_val;
	}

	auto c = b % a;
	int check_a = 0;
	int check_b = 7;
	for (auto&& i : c) {
		check_a += 1;
		check_b += 77;
		if (i != check_b % check_a) {
			return false;
		}
	}
	return true;
}


bool test_raster_plus_constant()
{
	int rows = 3;
	int cols = 5;
	auto a = pr::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	int constant = 100;

	auto c = a + constant;
	int check = 100;
	for (auto&& i : c) {
		check += 1;
		if (i != check) {
			return false;
		}
	}
	return check == (rows * cols + 100);
}

bool test_raster_times_constant()
{
	int rows = 3;
	int cols = 5;
	auto a = pr::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	int constant = 100;
	auto aa = pr::erase_and_hide_raster_type(a);

	auto cc = aa * constant;
	auto c = std::get<4>(cc.m_raster);
	int check = 0;
	for (auto&& i : c) {
		check += 1;
		if (i != 100 * check) {
			return false;
		}
	}
	return check == rows * cols;
}

bool test_three_operators()
{
	int rows = 3;
	int cols = 5;

	auto raster_a = pr::erase_and_hide_raster_type(pr::create_temp<int>(rows, cols));
	auto raster_b = pr::erase_and_hide_raster_type(pr::create_temp<int>(rows, cols));
	auto raster_c = pr::erase_and_hide_raster_type(pr::create_temp<int>(rows, cols));
	auto raster_out = pr::erase_and_hide_raster_type(pr::create_temp<int>(rows, cols));

	auto raster_sum = 3 * raster_a + raster_b * raster_c;

	pr::assign(raster_out, raster_sum);

	return true;
}
bool test_constant_plus_raster()
{
	int rows = 3;
	int cols = 5;
	auto a = pr::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	int constant = 100;

	auto c = constant + a;
	int check = 100;
	for (auto&& i : c) {
		check += 1;
		if (i != check) return false;
	}
	return check == (rows * cols + 100);
}

bool test_constant_plus_any_blind_raster()
{
	int rows = 3;
	int cols = 5;
	auto a = pr::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	int constant = 100;

	auto a_blind = pr::erase_and_hide_raster_type(a);
	auto c_blind = constant + a_blind;

	auto check = 100;

	auto c = std::get<4>(c_blind.m_raster); // int
	for (auto&& i : c) {
		check += 1;
		if (i != check) return false;
	}
	
	static_assert(pr::RasterVariantConcept<decltype(a_blind)>);

	return check == (rows * cols + 100);
	
}

bool test_any_blind_raster_plus_any_blind_raster()
{
	int rows = 3;
	int cols = 5;
	auto a = pr::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	auto b = pr::create_temp<int>(rows, cols);
	int hundreds = 0;
	for (auto&& i : b) {
		hundreds += 100;
		i = hundreds;
	}
	auto a_blind = pr::erase_and_hide_raster_type(a);
	auto b_blind = pr::erase_and_hide_raster_type(b);

	auto c_blind = a_blind + b_blind;
	auto c = std::get<4>(c_blind.m_raster); // int
	int check = 0;
	for (auto&& i : c) {
		check += 101;
		if (i != check) {
			return false;
		}
	}
	return check == rows * cols * 101;
}


TEST(RasterTest, MapAlgebra) {
  EXPECT_TRUE(test_raster_plus_raster());
  EXPECT_TRUE(test_raster_mod_raster());
  EXPECT_TRUE(test_constant_plus_raster());
	EXPECT_TRUE(test_three_operators());

	EXPECT_TRUE(test_raster_times_constant());
  EXPECT_TRUE(test_raster_plus_constant());
  EXPECT_TRUE(test_constant_plus_any_blind_raster());
  EXPECT_TRUE(test_any_blind_raster_plus_any_blind_raster());
}

