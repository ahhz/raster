The following example is a simple case of creating a raster of 10 rows by 5 columns and filling it with random values between 1 and 6. 

```cpp
// example_1.cpp
#include <blink/raster/raster.h>
#include <random>

int main()
{
  namespace br = blink::raster;

  std::default_random_engine rng;
  std::uniform_int_distribution<int> dice(1, 6);

  auto raster = br::create<int>("demo.tif", 10, 5);
  for (auto&& v : raster) {
    v = dice(rng);
  }
  plot_raster(raster);
  return 0;
}
```
