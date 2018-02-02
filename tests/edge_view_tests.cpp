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

#include <pronto/raster/filesystem.h>
#include <pronto/raster/io.h>
#include <pronto/raster/gdal_raster_view.h>
#include <pronto/raster/edge_raster.h>

#include <vector>

namespace br = pronto::raster;
namespace fs = br::filesystem;

bool test_v_edge()
{
  auto r = br::create_temp<int>(3, 5);
  int count = 0;
  for (auto&& i : r) {
    i = count++;
  }

  auto t = br::v_edge(r);
  
  std::vector < std::pair<int, int> > check_vector;
  for (auto&& i : t) {
    check_vector.push_back(i);
  }
  std::vector < std::pair<int, int> > c2;
  c2.emplace_back(0,1);
  c2.emplace_back(1,2);
  c2.emplace_back(2,3);
  c2.emplace_back(3, 4);
  c2.emplace_back(5, 6);
  c2.emplace_back(6, 7);
  c2.emplace_back(7, 8);
  c2.emplace_back(8, 9);
  c2.emplace_back(10, 11);
  c2.emplace_back(11, 12);
  c2.emplace_back(12, 13);
  c2.emplace_back(13, 14);
  
  return c2==check_vector;
}


bool test_h_edge()
{
  auto r = br::create_temp<int>(3, 5);
  int count = 0;
  for (auto&& i : r) {
    i = count++;
  }

  auto t = br::h_edge(r);

  std::vector < std::pair<int, int> > check_vector;
  for (auto&& i : t) {
    check_vector.push_back(i);
  }
  std::vector < std::pair<int, int> > c2;
  c2.emplace_back(0,5);
  c2.emplace_back(1, 6);
  c2.emplace_back(2, 7);
  c2.emplace_back(3, 8);
  c2.emplace_back(4, 9);
  c2.emplace_back(5, 10);
  c2.emplace_back(6, 11);
  c2.emplace_back(7, 12);
  c2.emplace_back(8, 13);
  c2.emplace_back(9, 14);

  return c2 == check_vector;
}

TEST(RasterTest, EdgeRaster) {
  EXPECT_TRUE(test_v_edge());
  EXPECT_TRUE(test_h_edge());
 }

