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
#include <ranges>
#include <vector>

namespace pr = pronto::raster;
namespace fs = std::filesystem;

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



bool test_sub_raster()
{
  int rows = 6;
  int cols = 4;
  auto a = pr::create_temp<int>(rows, cols);

  static_assert(std::ranges::range<decltype(a)>);
  static_assert(std::ranges::view<decltype(a)>);
  static_assert(std::ranges::random_access_range<decltype(a)>);
  static_assert(!std::ranges::contiguous_range<decltype(a)>);

  int num = 0;
  for (auto&& i : a) {
    num += 2;
    i = num;
  }
  auto c = a.sub_raster(2, 1, 3, 2);

  static_assert(std::is_same_v<decltype(a), decltype(c)>);
 
  std::vector<int> check;
  for (auto&& i : c) {
    check.push_back(i);
  }
  return check == std::vector<int>{20, 22, 28, 30, 36, 38};
}

  bool test_random_access_iteration()
  {
    int rows = 6;
    int cols = 4;
    auto a = pr::create_temp<int>(rows, cols);
    int num = 0;
    for (auto&& i : a) {
      num += 2;
      i = num;
    }
    auto c = a.sub_raster(2, 1, 3, 2);
    std::vector<int> check;
    for (auto&& i : c) {
      check.push_back(i);
    }

    auto t1 = *(c.begin() + 2) == *(check.begin() + 2);
    auto t2 = c.begin()[4] == check[4];
    auto t3 = *(--(c.begin() + 3)) == *(--(check.begin() + 3));
    auto t4 = *(++(c.begin() + 3)) == *(++(check.begin() + 3));
    auto t5 = *((c.begin() + 3) - 1) == *((check.begin() + 3) - 1);
    auto i = c.begin() + 2;
    auto i_check = check.begin() + 2;
    auto t6 = *(++i) == *(++i_check);
    auto t7 = *(i++) == *(i_check++);
    auto t8 = *(i) == *(i_check);
    auto t9 = *(--c.end()) == *(--check.end());
    auto t10 = *(c.end()-2) == *(check.end()-2); 
    return t1 && t2 && t3 && t4 && t5 && t6 && t7 && t8 && t9 && t10;
}

bool test_empty_gdal_raster_view()
{
  auto r = pr::create_temp<int>(1, 1).sub_raster(0, 0, 0, 0);
  for (auto&& i : r) {
    return false;
  }
  return r.size() == 0 && r.rows() == 0 && r.cols() == 0;
}

bool test_empty_gdal_raster_view_zero_rows()
{
  auto r = pr::create_temp<int>(1, 1).sub_raster(0, 0, 0, 1);
  for (auto&& i : r) {
    return false;
  }
  return r.size() == 0 && r.rows() == 0 && r.cols() == 1;
}

bool test_empty_gdal_raster_view_zero_cols()
{
  auto r = pr::create_temp<int>(1, 1).sub_raster(0, 0, 1, 0);
  for (auto&& i : r) {
    return false;
  }
  return r.size() == 0 && r.rows() == 1 && r.cols() == 0;
}

TEST(RasterTest, ReferenceProxy) {
  EXPECT_TRUE(test_assign_reference_proxy());
  EXPECT_TRUE(test_increment_reference_proxy());
}

TEST(RasterTest, GdalRasterView) {
  EXPECT_TRUE(test_sub_raster());
  EXPECT_TRUE(test_random_access_iteration());
  EXPECT_TRUE(test_empty_gdal_raster_view());
  EXPECT_TRUE(test_empty_gdal_raster_view_zero_rows());
  EXPECT_TRUE(test_empty_gdal_raster_view_zero_cols());

}
