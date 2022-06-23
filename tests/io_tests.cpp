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
#include <pronto/raster/raster_algebra_operators.h>
#include <pronto/raster/raster_variant.h>

#include <filesystem>
#include <variant>
#include <vector>

namespace pr = pronto::raster;
namespace fs = std::filesystem;

bool test_access_settings()
{
  auto s1 = pr::access_settings(pr::read_only, pr::single_pass);

  return true;
}
bool test_create_temp()
{
  auto r = pr::create_temp<int>(5, 3);
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

bool test_create_temp_uncasted()
{
  auto r = pr::create_temp_uncasted<int>(5, 3);
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
    auto r = pr::create<int>("temp.tif", 5, 3);
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
    auto r = pr::create<int>("temp.tif", 5, 3);
    int count = 0;
    for (auto&& i : r) {
      i = count++;
    }
  } // leave scope
  bool check_exist = fs::exists("temp.tif");
  bool check_contents;
  {
    auto r = pronto::raster::open<int, pr::iteration_type::multi_pass, pr::access::read_only>("temp.tif");
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

bool test_open_variant()
{
  {
    auto r = pr::create<int>("temp.tif", 5, 3);
    int count = 0;
    for (auto&& i : r) {
      i = count++;
    }
  } // leave scope
  bool check_exist = fs::exists("temp.tif");
  bool check_contents;
  {
    auto r = pronto::raster::open_variant("temp.tif");
    //auto f = [](auto r1, auto r2) {return pr::erase_and_hide_raster_type(pr::transform(std::plus<int>{}, pr::optional_to_nodata(r1,0), pr::optional_to_nodata(r2, 0))); };
    //auto two_r = std::visit(f, r.m_raster,r.m_raster);
    auto two_r = r + r;
    std::vector<int> check_vector;
    auto copy = [&](auto ras) {

      using value_type = pr::recursive_optional_value_type<decltype(ras)::iterator::value_type>;
      auto r2 = pr::optional_to_nodata(ras, static_cast<value_type>(- 999));
      for (auto&& v : r2)
      {
        check_vector.push_back(static_cast<int>(v));
      };
    };
    std::visit(copy, two_r.m_raster);

    check_contents = check_vector == std::vector<int>
    {0, 2,4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28};
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
    auto view = pr::create<int>("temp.tif", rows, cols);
    int count = 0;
    for (auto&& i : view) {
      i = count++;
    }
  }
  bool check_exist = fs::exists("temp.tif");

  bool check_contents;
  {
    auto view = pr::open<int, pr::iteration_type::multi_pass, pr::access::read_only>("temp.tif");

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
  EXPECT_TRUE(test_access_settings());
  EXPECT_TRUE(test_create_temp());
  EXPECT_TRUE(test_create_temp_uncasted());
  EXPECT_TRUE(test_create());
  EXPECT_TRUE(test_open());
  EXPECT_TRUE(test_open_variant());
#ifdef NDEBUG // Don't debug large data file
  EXPECT_TRUE(test_create_open_large());
#endif
}
