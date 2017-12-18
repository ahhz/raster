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
Output (will vary due to random number generator):
```
Rows: 10, Cols: 5, Value type: int
3       1       3       6       5
2       6       6       1       2
2       6       1       1       6
1       4       5       2       5
6       2       6       2       1
3       5       4       5       6
1       4       2       3       1
2       2       6       2       1
1       1       6       1       6
1       4       5       2       3
```