//example_transform.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/transform_raster_view.h>

namespace pr = pronto::raster;

template<class T>
T join(const T& a, const T& b, const T& c)
{
  return 100 * a + 10 * b + c;
}

struct sum
{
  template<class T>
  T operator()(const T& a, const T& b, const T& c) const
  {
    return a + b + c;
  }
};

int main()
{
  auto product = [](int a, int b, int c) { return a * b * c;  };
  
  auto aa = pr::create_temp<int>(4, 5);
  auto bb = pr::create_temp<int>(4, 5);
  auto cc = pr::create_temp<int>(4, 5);
  int i = 0;
  for (auto&& v : aa) {
    v = ((i += 3) %= 7);
  }

  for (auto&& v : bb) {
    v = ((i += 3) %= 7);
  }

  for (auto&& v : cc) {
    v = ((i += 3) %= 7);
  }

  auto join_abc = pr::transform(join<int>, aa, bb, cc);
  auto sum_abc = pr::transform(sum{}, aa, bb, cc);
  auto product_abc = pr::transform(product, aa, bb, cc);

  plot_raster(join_abc);
  plot_raster(sum_abc);
  plot_raster(product_abc);

  return 0;
}