//
//=======================================================================
// Copyright 20222
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include <gtest/gtest.h>

#include <pronto/raster/io.h>
#include <pronto/raster/padded_raster_view.h>
#include <pronto/raster/raster.h>
namespace pr = pronto::raster;

bool test_padded_raster()
{
  int rows = 3;
  int cols = 2;
  auto a = pr::create_temp<int>(rows, cols);
  for (int i = 0; auto && v : a)
  {
    v = ++i;
  }
  auto leading_rows = 1;
  auto trailing_rows = 2;
  auto leading_cols = 3;
  auto trailing_cols = 4;
  auto pad_value = 0;

  auto pa = pr::pad(a, leading_rows, trailing_rows, leading_cols, trailing_cols, pad_value);
 
  static_assert(pr::RasterConcept<decltype(a)>);
  static_assert(pr::RasterConcept<decltype(pa)>);

  std::vector<int> vec;
  for (auto && v : pa)
  {
    vec.push_back(v);  
  }
  auto vec2 = std::vector<int>{
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 2, 0, 0, 0, 0,
    0, 0, 0, 3, 4, 0, 0, 0, 0,
    0, 0, 0, 5, 6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0 };
  
  return vec == vec2;;
}

bool test_padded_raster_trailing_cols()
{
  int rows = 3;
  int cols = 2;
  auto a = pr::create_temp<int>(rows, cols);
  for (int i = 0; auto && v : a)
  {
    v = ++i;
  }
  auto leading_rows = 0;
  auto trailing_rows = 0;
  auto leading_cols = 0;
  auto trailing_cols = 2;
  auto pad_value = 0;

  auto pa = pr::pad(a, leading_rows, trailing_rows, leading_cols, trailing_cols, pad_value);

  static_assert(pr::RasterConcept<decltype(a)>);
  static_assert(pr::RasterConcept<decltype(pa)>);

  std::vector<int> vec;
  for (auto&& v : pa)
  {
    vec.push_back(v);
  }
  auto vec2 = std::vector<int>{ 
    1,2,0,0,
    3,4,0,0,
    5,6,0,0};

  return vec == vec2;;
}

bool test_padded_raster_leading_cols()
{
  int rows = 3;
  int cols = 2;
  auto a = pr::create_temp<int>(rows, cols);
  for (int i = 0; auto && v : a)
  {
    v = ++i;
  }
  auto leading_rows = 0;
  auto trailing_rows = 0;
  auto leading_cols = 2;
  auto trailing_cols = 0;
  auto pad_value = 0;

  auto pa = pr::pad(a, leading_rows, trailing_rows, leading_cols, trailing_cols, pad_value);

  static_assert(pr::RasterConcept<decltype(a)>);
  static_assert(pr::RasterConcept<decltype(pa)>);

  std::vector<int> vec;
  for (auto&& v : pa)
  {
    vec.push_back(v);
  }
  auto vec2 = std::vector<int>{
    0,0,1,2,
    0,0,3,4,
    0,0,5,6 };

  return vec == vec2;;
}
bool test_padded_raster_trailing_rows()
{
  int rows = 3;
  int cols = 2;
  auto a = pr::create_temp<int>(rows, cols);
  for (int i = 0; auto && v : a)
  {
    v = ++i;
  }
  auto leading_rows = 0;
  auto trailing_rows = 2;
  auto leading_cols = 0;
  auto trailing_cols = 0;
  auto pad_value = 0;

  auto pa = pr::pad(a, leading_rows, trailing_rows, leading_cols, trailing_cols, pad_value);

  static_assert(pr::RasterConcept<decltype(a)>);
  static_assert(pr::RasterConcept<decltype(pa)>);

  std::vector<int> vec;
  for (auto&& v : pa)
  {
    vec.push_back(v);
  }
  auto vec2 = std::vector<int>{
    1,2,
    3,4,
    5,6,
    0,0,
    0,0 };

  return vec == vec2;;
}

bool test_padded_raster_leading_rows()
{
  int rows = 3;
  int cols = 2;
  auto a = pr::create_temp<int>(rows, cols);
  for (int i = 0; auto && v : a)
  {
    v = ++i;
  }
  auto leading_rows = 2;
  auto trailing_rows = 0;
  auto leading_cols = 0;
  auto trailing_cols = 0;
  auto pad_value = 0;

  auto pa = pr::pad(a, leading_rows, trailing_rows, leading_cols, trailing_cols, pad_value);

  static_assert(pr::RasterConcept<decltype(a)>);
  static_assert(pr::RasterConcept<decltype(pa)>);

  std::vector<int> vec;
  for (auto&& v : pa)
  {
    vec.push_back(v);
  }
  auto vec2 = std::vector<int>{
    0,0,
    0,0,
    1,2,
    3,4,
    5,6 };

  return vec == vec2;;
}
TEST(RasterTest, PaddedRaster) {
  EXPECT_TRUE(test_padded_raster());
  EXPECT_TRUE(test_padded_raster_leading_rows());
  EXPECT_TRUE(test_padded_raster_leading_cols());
  EXPECT_TRUE(test_padded_raster_trailing_rows());
  EXPECT_TRUE(test_padded_raster_trailing_cols());
}
