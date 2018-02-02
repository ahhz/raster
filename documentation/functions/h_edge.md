# h_edge
## Prototype
```cpp
template<class Raster>
h_edge_view<Raster> h_edge(Raster raster);
```
## Description
Create a view of the input raster that iterates over all horizontal edges in the raster. A horizontal edge is formed by the vertically adjacent cells in a raster. The value of the edge is a std::pair of the two values found in the top (first) and bottom (second) of the two adjacent cells. When rows_in and cols_in are the dimensions of the input raster, than rows = rows_in -1 and cols = cols_in are the dimensions of the output raster. The value_type of h_edge_view is std::pair<in_value_type, in_value_type>.

h_edge_view<Raster> is a typedef for a class that implements the RasterView concept. If the iterator of Raster is a RandomAccessibleRasterIterator then that of h_edge_view<Raster> is too.

## Definition
[<pronto/raster/io.h>](./../../include/pronto/raster/edge_raster.h)

## Requirements on types
Raster must implement [RasterView](.\..\concepts\raster_view.md)

## Preconditions
Raster must be initialized.

## Complexity
Independent of raster size, O(1)

## Example of use
```cpp//example_h_edge.cpp
#include <pronto/raster/edge_raster.h>
#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>


namespace br = pronto::raster;

int main()
{
  auto raster = br::create_temp<unsigned char>(3, 4);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }
  
  auto edges = br::h_edge(raster);
 
  plot_raster(raster);
  plot_raster(edges);
  
  return 0;
}
```
Output:
```
Rows: 3, Cols: 4.
3       6       2       5
1       4       0       3
6       2       5       1

Rows: 2, Cols: 4.
{3, 1}  {6, 4}  {2, 0}  {5, 3}
{1, 6}  {4, 2}  {0, 5}  {3, 1}
```
## Notes
## See also
[v_edge](./v_edge.md)