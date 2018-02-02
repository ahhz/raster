The following example is a simple case of opening a raster dataset and calculating the average value of all cells.
```cpp
// example_2.cpp
#include <pronto/raster/io.h>
#include <iostream>

namespace br = pronto::raster;

int main()
{
  auto raster = br::open<int>("demo.tif"); // assuming file exists and is valid
  int sum = 0;
  for (auto&& value : raster) {
    sum += value;
  }
  std::cout << "Mean value: " << static_cast<double>(sum) / raster.size() << std::endl;
 
  return 0;
}
```

Output (depending on contents of "demo.tif"; here using file generated in [example_1](./example_1.md) ):
```
Mean value: 3.24
```
