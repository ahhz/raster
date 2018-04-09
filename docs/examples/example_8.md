# Use the optional value type for nodata values
Nodata values are a common occurance in geographical data. Often a special value is used to indicate an absence of data. This is called the nodata value. In modern C++, the `optional<T>` class is used for objects that may be in an uninitialized state (either boost::optional<T> or std::optional<T> in C++17). The following example shows how the Pronto Raster library helps working with nodata values. First, a data-set is opened (`in`) then a view is created (`nodata`) that has the value type `optional<int>` and treats the cells with value 6 as unitialized. Finally a view is created, with `int` as value_type again that assigns the value of -99 to the unitialized values. Thus the user of the library has full control of the treatment of nodata values. 
```cpp
//example_8.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/nodata_transform.h>
#include <pronto/raster/plot_raster.h>

namespace pr = pronto::raster;

int main()
{
  // Create some data
  auto in = pr::create_temp<int>(4,5);
  int i = 0;
  for (auto&& v : in) {
    v = i;
    i = (i + 3) % 7;
  }

  // Treat value 6 as nodata
  auto nodata = pr::nodata_to_optional(in, 6);

  // Treat nodata as value -99
  auto un_nodata = pr::optional_to_nodata(nodata, -99);

  plot_raster(in);
  plot_raster(nodata);
  plot_raster(un_nodata);
  
  return 0;
}
```

Output: 
```
Rows: 4, Cols: 5, Value type: int
0       3       6       2       5
1       4       0       3       6
2       5       1       4       0
3       6       2       5       1

Rows: 4, Cols: 5, Value type: class std::optional<int>
0       3       -       2       5
1       4       0       3       -
2       5       1       4       0
3       -       2       5       1

Rows: 4, Cols: 5, Value type: int
0       3       -99     2       5
1       4       0       3       -99
2       5       1       4       0
3       -99     2       5       1
```