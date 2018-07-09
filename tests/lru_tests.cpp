#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include <gtest/gtest.h>

#include <pronto/raster/random_raster_view.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/self_cached_gdal_raster_view.h>
#include <random>
#include <vector>
#include <map>

namespace pr = pronto::raster;

bool lru_test()
{
  pr::lru my_lru(10u);
  using id = typename pr::lru::id;
  std::vector<int> removed;
  id id1 = my_lru.add(4, [&]() {removed.push_back(1); });
  id id2 = my_lru.add(3, [&]() {removed.push_back(2); });
  id id3 = my_lru.add(2, [&]() {removed.push_back(3); });
  std::vector<int> check;
  
  // (1,4; 2,3; 3,2)
  if (removed!=check) return false;
  id id4 = my_lru.add(4, [&]() {removed.push_back(4); });
  // (2,3; 3,2; 4,4)

  check.push_back(1);
  if (removed != check) return false;

  my_lru.touch(id2);
  //( 3,2; 4,4; 2,3)
  id id5 = my_lru.add(2, [&]() {removed.push_back(5); });
  //( 4,4; 2,3; 5,2)
  check.push_back(3);
  if (removed != check) return false;
  id id6 = my_lru.add(7, [&]() {removed.push_back(6); });
  //( 5,2; 6,7)
  check.push_back(4);
  check.push_back(2);
  if (removed != check) return false;
  return true;
}
bool cached_gdal_raster_test()
{
  auto raster = pr::open_v2<int, false, false>("random_a.tif");
  return true;
}

bool create_cached_gdal_raster_test()
{
  auto raster = pr::create_v2<unsigned char>("test.tif", 10, 5);
  int i = 3;
  for (auto&& v : raster)
  {
    v = i;
    i = (i + 6) % 7;
  }
  pr::plot_raster(raster);
  return true;
}


bool larger_create_cached_gdal_raster_test()
{
  int rows =300 ;
  int cols = 300;
  auto a = pr::create_v2<unsigned char,true>("a.tif", rows, cols);
  auto b = pr::create_v2<unsigned char, true>("b.tif", rows, cols);
  auto c = pr::create_v2<unsigned char, true>("c.tif", rows, cols);
  
  auto ia = a.begin();
  auto ib = b.begin();
  auto ic = c.begin();
  int i = 0;
  for (; ia != a.end(); ++ia, ++ib,++ic)
  {
    i = i + 3;
    *ia = i % 23;
    *ib = i % 7;
    *ic = *ia + *ib;
  }

  return true;
}

int main()
{
  int rows = 600;
  int cols = 600;
  auto a = pr::create_v2<unsigned char, true>("a.tif", rows, cols);
  auto b = pr::create_v2<unsigned char, true>("b.tif", rows, cols);
  auto c = pr::create_v2<unsigned char, true>("c.tif", rows, cols);

  auto ia = a.begin();
  auto ib = b.begin();
  auto ic = c.begin();
  int i = 0;
  for (; ia != a.end(); ++ia, ++ib, ++ic)
  {
    i = i + 3;
    *ia = i % 23;
    *ib = i % 7;
    *ic = *ia + *ib;
  }

  return 0;
}


TEST(RasterTest, Lru) {
	EXPECT_TRUE(lru_test());
 // EXPECT_TRUE(cached_gdal_raster_test());
  EXPECT_TRUE(create_cached_gdal_raster_test());
  EXPECT_TRUE(larger_create_cached_gdal_raster_test());
}