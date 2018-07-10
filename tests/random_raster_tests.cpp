#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include <gtest/gtest.h>

#include <pronto/raster/random_raster_view.h>
#include <pronto/raster/plot_raster.h>
#include <random>
#include <vector>

namespace pr = pronto::raster;

bool create_random_raster_view()
{
  auto r = pr::random_distribution_raster(5, 5, std::uniform_int_distribution<>(1, 6), std::mt19937());
  int count = 0;
  for (auto&& i : std::as_const(r))
  {
    ++count;
    if (i < 1 || i > 6) return false;
  }
  return count == 25;
}

bool sub_raster_random_raster_view()
{
  assert(pr::g_lru.empty());
  int n = 500;
  auto r = pr::random_distribution_raster(2 * n, 2 * n, 
    std::uniform_int_distribution<>(1, 6), std::mt19937());
    
  int count = 0;
  auto r1 = r.sub_raster(0, 0, n, n);
  auto r2 = r.sub_raster(0, n, n, n);
  auto r3 = r.sub_raster(n, 0, n, n);
  auto r4 = r.sub_raster(n, n, n, n);

  auto i = r.begin();
  auto i1 = r1.begin();
  auto i2 = r2.begin();
  auto i3 = r3.begin();
  auto i4 = r4.begin();

  for (int row = 0; row < 2 * n; ++row) {
    for (int col = 0; col < 2 * n; ++col)
    {
      int segment = 1 + 2 * (row / n) + col / n;
      switch (segment)
      {
      case 1:
        if (*i != *i1)
          return false;
        ++i1;
        break;
      case 2:
        if (*i != *i2)
          return false;
        ++i2;
        break;
      case 3:
        if (*i != *i3)
          return false;
        ++i3;
        break;
      case 4:
        if (*i != *i4)
          return false;
        ++i4;
        break;
      default:
        return false;
      }
      ++i;
    }
  }
  return true;
}

bool sub_sub_raster_random_raster_view()
{
  int n = 500;
  auto r = pr::random_distribution_raster(2 * n, 2 * n, std::uniform_int_distribution<>(1, 6), std::mt19937());
  int count = 0;
  auto r1 = r.sub_raster(0, 0, n, n);
  auto r2 = r.sub_raster(0, n, n, n);
  auto r3 = r.sub_raster(n, 0, n, n);
  auto r4a = r.sub_raster(n - n / 2, n - n / 2, n + n / 2, n + n / 2);
  auto r4 = r4a.sub_raster(n / 2, n / 2, n, n);

  auto i = r.begin();
  auto i1 = r1.begin();
  auto i2 = r2.begin();
  auto i3 = r3.begin();
  auto i4 = r4.begin();

  for (int row = 0; row < 2 * n; ++row) {
    for (int col = 0; col < 2 * n; ++col)
    {
      int segment = 1 + 2 * (row / n) + col / n;
      switch (segment)
      {
      case 1:
        if (*i != *i1)
          return false;
        ++i1;
        break;
      case 2:
        if (*i != *i2)
          return false;
        ++i2;
        break;
      case 3:
        if (*i != *i3)
          return false;
        ++i3;
        break;
      case 4:
        if (*i != *i4)
          return false;
        ++i4;
        break;
      default:
        return false;
      }
      ++i;
    }
  }
  return true;
}

bool test_based_on_benchmark()
{
  int rows = 10000; 
  int cols = 10000;
  std::cout << "LRU Cap: " << pr::g_lru.get_capacity() << "size " << pr::g_lru.get_size() <<  std::endl;
  auto raster_a = pr::create<int>("random_a.tif", rows, cols, GDT_Byte);
 // auto raster_b = pr::create<int>("random_b.tif", rows, cols, GDT_Byte);
  auto dist = std::uniform_int_distribution<int>(1, 6);
  auto gen_a = std::mt19937(std::random_device{}());
  auto random_a = pr::random_distribution_raster(rows, cols, dist, gen_a);
  pr::assign(raster_a, random_a);
  return true;
}


TEST(RasterTest, RandomRasterView) {
	 EXPECT_TRUE(create_random_raster_view());
   EXPECT_TRUE(sub_raster_random_raster_view());
   EXPECT_TRUE(sub_sub_raster_random_raster_view());
   EXPECT_TRUE(test_based_on_benchmark());
}