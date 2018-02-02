//example_9.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/subraster_window_view.h>

namespace pr = pronto::raster;

int main()
{
  auto in = pr::open<int>("demo.tif");
  
  int radius = 1;
  int pad_value = -99;

  auto window = pr::make_square_subraster_window_view(in, radius, pad_value);

  plot_raster(in);

  // to avoid plotting too much, only plot only the windows for the cells 
  // in the first two rows and three columns.
  for (auto&& i : window.sub_raster(0, 0, 2, 3) ) {
    plot_raster(i);
  }
  
  return 0;
}