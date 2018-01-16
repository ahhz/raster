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

#include <blink/raster/filesystem.h>
#include <blink/raster/io.h>
#include <blink/raster/gdal_raster_view.h>

#include <vector>

namespace br = blink::raster;
namespace fs = br::filesystem;

bool test_create_temp()
{
  auto r = br::create_temp<int>(5, 3);
  int count = 0;
  for (auto&& i : r) {
    i = count++;
  }
  std::vector<int> check_vector;
  for (auto&& i : r) {
    check_vector.push_back(i);
  }
  return check_vector == std::vector<int>
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
}

bool test_create()
{
  {
    auto r = br::create<int>("temp.tif", 5, 3);
    int count = 0;
    for (auto&& i : r) {
      i = count++;
    }
  } // leave scope
  bool check_exist = fs::exists("temp.tif");
  fs::remove("temp.tif");
  bool check_not_exist = !fs::exists("temp.tif");
  return check_exist && check_not_exist;
}

bool test_open()
{
  {
    auto r = br::create<int>("temp.tif", 5, 3);
    int count = 0;
    for (auto&& i : r) {
      i = count++;
    }
  } // leave scope
  bool check_exist = fs::exists("temp.tif");
  bool check_contents;
  {
    auto r = blink::raster::open<int>("temp.tif", br::access::read_only);
    std::vector<int> check_vector;
    for (auto&& i : r) {
      check_vector.push_back(i);
    }
    check_contents =  check_vector == std::vector<int>
     {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
  }
  fs::remove("temp.tif");
  bool check_not_exist = !fs::exists("temp.tif");
  return check_exist && check_not_exist && check_contents;
}

bool test_create_open_large()
{
  int rows = 2000;
  int cols = 1500;
  {
    auto view = br::create<int>("temp.tif", rows, cols);
    int count = 0;
    for (auto&& i : view) {
      i = count++;
    }
  }
  bool check_exist = fs::exists("temp.tif");

  bool check_contents;
  {
    auto view = br::open<int>("temp.tif", br::access::read_only);

    std::vector<int> check_vector;
    for (auto&& i : view) {
      check_vector.push_back(i);
    }
    int check_last = check_vector.back() == (rows * cols - 1);
    check_vector.resize(15);
    check_contents = check_vector == std::vector<int>
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}
    && check_last;
  }
  fs::remove("temp.tif");
  bool check_not_exist = !fs::exists("temp.tif");
  return check_exist && check_not_exist && check_contents;
}

TEST(RasterTest, IO) {
  EXPECT_TRUE(test_create_temp());
  EXPECT_TRUE(test_create());
  EXPECT_TRUE(test_open());
#ifdef NDEBUG // Don't debug large data file
  EXPECT_TRUE(test_create_open_large());
#endif
}
