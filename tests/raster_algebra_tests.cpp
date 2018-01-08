#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include <gtest/gtest.h>

#include <blink/raster/io.h>
#include <blink/raster/raster_algebra_wrapper.h>
#include <blink/raster/raster_algebra_operators.h>

#include <vector>

namespace br = blink::raster;
namespace fs = br::filesystem;

bool test_raster_plus_raster()
{
	int rows = 3;
	int cols = 5;
	auto a = br::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	auto b = br::create_temp<int>(rows, cols);
	int hundreds = 0;
	for (auto&& i : b) {
		hundreds += 100;
		i = hundreds;
	}

	auto c = br::raster_algebra_wrap(a) + br::raster_algebra_wrap(b);
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
	auto a = br::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	auto b = br::create_temp<int>(rows, cols);
	int b_val = 7;
	for (auto&& i : b) {
		b_val += 77;
		i = b_val;
	}

	auto c = br::raster_algebra_wrap(b) % br::raster_algebra_wrap(a);
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
	auto a = br::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	int constant = 100;

	auto c = br::raster_algebra_wrap(a) + constant;
	int check = 100;
	for (auto&& i : c) {
		check += 1;
		if (i != check) {
			return false;
		}
	}
	return check == (rows * cols + 100);
}

bool test_constant_plus_raster()
{
	int rows = 3;
	int cols = 5;
	auto a = br::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	int constant = 100;

	auto c = constant + br::raster_algebra_wrap(a);
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
	auto a = br::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	int constant = 100;

	auto a_blind = br::make_any_blind_raster(a);
	auto c_blind = constant + br::raster_algebra_wrap(a_blind);

	auto c = c_blind.unwrap().get_by_type<int>();
	static_assert(
		std::is_same<decltype(c), br::any_raster<int> >::value,
		"checking logic of any_blind_raster");

	int check = 100;
	for (auto&& i : c) {
		check += 1;
		if (i != check) return false;
	}
	return check == (rows * cols + 100);
}

bool test_any_blind_raster_plus_any_blind_raster()
{
	int rows = 3;
	int cols = 5;
	auto a = br::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	auto b = br::create_temp<int>(rows, cols);
	int hundreds = 0;
	for (auto&& i : b) {
		hundreds += 100;
		i = hundreds;
	}
	auto a_blind = br::make_any_blind_raster(a);
	auto b_blind = br::make_any_blind_raster(b);

	auto c_blind = br::raster_algebra_wrap(a_blind) + 
		br::raster_algebra_wrap(b_blind);
	auto c = c_blind.unwrap().get_by_type<int>();
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
  EXPECT_TRUE(test_raster_plus_constant());
  EXPECT_TRUE(test_constant_plus_any_blind_raster());
  EXPECT_TRUE(test_any_blind_raster_plus_any_blind_raster());
}

