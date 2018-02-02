//example_raster_algebra_transform.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/raster_algebra_transform.h>

namespace br = pronto::raster;

template<class T>
T join(const T& a, const T& b, const T& c)
{
  return 100 * a + 10 * b + c;
}

struct sum
{
  template<class T>
  T operator()(const T& a, const T& b, const T& c) 
  {
    return a + b + c;
  }
};

int main()
{
  auto product = [](int a, int b, int c) { return a * b * c;  };
  
  auto a = br::create_temp<int>(4, 5);
  auto b = br::create_temp<int>(4, 5);
  auto c = 5;

  int i = 0;
  for (auto&& v : a) {
    v = ((i += 3) %= 7);
  }

  for (auto&& v : b) {
    v = ((i += 3) %= 7);
  }

  auto wrapped_a = br::raster_algebra_wrap(a);
  auto wrapped_b = br::raster_algebra_wrap(b);

  auto join_abc = br::raster_algebra_transform(join<int>, wrapped_a, wrapped_b, c);
  auto sum_abc = br::raster_algebra_transform(sum{}, wrapped_a, wrapped_b, c);
  auto product_abc = br::raster_algebra_transform(product, wrapped_a, wrapped_b, c);
  
  plot_raster(join_abc);
  plot_raster(sum_abc);
  plot_raster(product_abc);

  return 0;
}