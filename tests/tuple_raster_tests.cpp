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

  return true;
}





TEST(RasterTest, TupleRaster) {
  EXPECT_TRUE(test_basic_tuple_raster());
}
