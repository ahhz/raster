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
#include <pronto/raster/tuple_raster_view.h>
namespace pr = pronto::raster;

bool test_basic_tuple_raster()
{
  int rows = 3;
  int cols = 2;
  auto a = pr::create_temp<int>(rows, cols);
  auto b = pr::create_temp<double>(rows, cols);
  auto c = pr::create_temp<int>(rows, cols);
  auto ab = pr::raster_tuple(a, b);
  auto abc = pr::raster_tuple(a, b, c);


  int v = 0;
  for (auto&& i : ab)
  {
    ++v;
    std::get<0>(i) = v;
    std::get<1>(i) = 100 * v;
  }
  for (auto&& i : abc)
  {
    std::get<2>(i) = std::get<0>(i) + std::get<1>(i);
  }
  std::vector<int> vec;
  for (auto&& i : c)
  {
    vec.push_back(i);
  }
  return vec == std::vector<int>{101, 202, 303, 404, 505, 606};
}


bool test_basic_tuple_raster_subraster()
{
  int rows = 8;
  int cols = 7;
  auto a = pr::create_temp<int>(rows, cols);
  auto b = pr::create_temp<double>(rows, cols);
  auto c = pr::create_temp<int>(rows, cols);
  auto ab = pr::raster_tuple(a, b);
  auto abc = pr::raster_tuple(a, b, c);


  int v = 0;
  for (auto&& i : ab)
  {
    ++v;
    std::get<0>(i) = v;
    std::get<1>(i) = 100 * v;
  }
  auto sub_abc = abc.sub_raster(2, 3, 4, 3);
  for (auto&& i : sub_abc)
  {
    std::get<2>(i) = std::get<0>(i) + std::get<1>(i);
  }
  std::vector<int> vec;
  for (auto&& i : c.sub_raster(2, 3, 4, 3))
  {
    vec.push_back(i);
  }
  return vec == std::vector<int>{
    1818, 1919, 2020, 
    2525, 2626, 2727, 
    3232, 3333, 3434,
    3939, 4040, 4141};
}


TEST(RasterTest, TupleRaster) {
  EXPECT_TRUE(test_basic_tuple_raster());
  EXPECT_TRUE(test_basic_tuple_raster_subraster());
}
