#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include <gtest/gtest.h>

#include <pronto/raster/random_raster_view.h>
#include <pronto/raster/plot_raster.h>
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


TEST(RasterTest, Lru) {
	EXPECT_TRUE(lru_test());
}