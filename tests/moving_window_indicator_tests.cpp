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
#include <pronto/raster/raster.h>
#include <pronto/raster/moving_window_indicator.h>
#include <pronto/raster/indicator/mean.h>
#include <pronto/raster/indicator/edge_density.h>

namespace pr = pronto::raster;

bool test_moving_window()
{
  int rows = 3;
  int cols = 2;
  auto a = pr::create_temp<int>(rows, cols);
  for (int i = 0; auto && v : a)
  {
    v = ++i;
  }

  auto w1 = pr::moving_window_indicator(a, pr::circle(1.5), pr::mean_generator<double>{});
  static_assert(pr::RasterConcept<decltype(w1)>);
  static_assert(std::ranges::input_range<decltype(w1)>);

  auto w2 = pr::moving_window_indicator(a, pr::square(1.5), pr::mean_generator<double>{});
  static_assert(pr::RasterConcept<decltype(w2)>);
  static_assert(std::ranges::input_range<decltype(w2)>);

  auto w3 = pr::moving_window_indicator(a, pr::edge_circle(1.5), pr::edge_density_generator<int>{});
  static_assert(pr::RasterConcept<decltype(w3)>);
  static_assert(std::ranges::input_range<decltype(w3)>);

  auto w4 = pr::moving_window_indicator(a, pr::edge_square(1.5), pr::edge_density_generator<int>{});
  static_assert(std::ranges::sized_range<decltype(w4)>);

//  template <class _Ty>
//  concept movable = is_object_v<_Ty>
//    && move_constructible<_Ty>
//    && assignable_from<_Ty&, _Ty>
//    && swappable<_Ty>;
  static_assert(std::move_constructible<decltype(w4)>);
  static_assert(std::assignable_from<decltype(w4)&, decltype(w4) >);

  static_assert(std::ranges::view<decltype(w4)>);

  static_assert(pr::RasterConcept<decltype(w4)>);
  static_assert(std::ranges::input_range<decltype(w4)>);

  return true;



}

TEST(RasterTest, MovingWindowIndicator) {
  EXPECT_TRUE(test_moving_window());
}
