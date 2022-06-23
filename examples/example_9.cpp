//example_9.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/optional_raster_view.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/subraster_window_view.h>

#include <random>

namespace pr = pronto::raster;

auto create_temporary_raster_for_demo()
{
  // See also example_1
  auto rng = std::default_random_engine{};
  auto dice = std::uniform_int_distribution<int>{ 1, 6 };

  auto raster = pr::create_temp<int>(10, 5);
  for (auto&& v : raster) {
    v = dice(rng);
  }
  return raster;
}

int main()
{
  auto in = create_temporary_raster_for_demo();
  
  auto radius = 1;

  // See the raster value_type as optional, so the padded edges can be uninitialized.
  auto optional_in = pr::optionalize(in);
  auto pad_value = std::optional<int>{};

  auto window = pr::make_square_subraster_window_view(optional_in, radius, pad_value);

  plot_raster(in);

  // to avoid plotting too much, only plot only the windows for the cells 
  // in the first two rows and three columns.
  for (auto&& i : window.sub_raster(0, 0, 2, 3) ) {
    plot_raster(i);
  }
  
  return 0;
}