//example_any_blind_raster.cpp

#include <pronto/raster/any_blind_raster.h>
#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/transform_raster_view.h>

namespace br = pronto::raster;

// the following function shows how any_blind_raster is used to decide 
// at runtime which value_type to use

template<class Raster> 
br::any_blind_raster optionally_take_square_root(Raster raster, bool take_root)
{
  using input_value_type = typename br::traits<Raster>::value_type;
  if (take_root) {

    // value type will be return type of sqrt<input_value_type>

    auto square_rooted = br::transform(std::sqrt<input_value_type>, raster);
    return br::make_any_blind_raster(square_rooted);
  }
  else {

    // value_type will be traits<Raster>::value_type

    return br::make_any_blind_raster(raster);
  }
}

int main()
{
  auto raster = br::create_temp<int>( 3, 4, GDT_Byte);
  auto i = 0;
  for (auto&& v : raster) {
    i = (i + 3) % 7;
    v = i;
  }

  br::any_blind_raster a = optionally_take_square_root(raster, false);
  br::any_blind_raster b = optionally_take_square_root(raster, true);

  plot_raster(a);
  plot_raster(b);

  return 0;
}