//example_blind_function.cpp

#include <pronto/raster/any_blind_raster.h>
#include <pronto/raster/blind_function.h>
#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/transform_raster_view.h>

namespace pr = pronto::raster;

struct square
{
public:
  template<class T>
  pr::any_blind_raster operator()(const pr::any_raster<T>& raster) const
  {
    auto sq_fun = [](const T& v) {return v * v; };
    auto result_typed = pr::transform(sq_fun, raster);

    // wrapping as any_blind_raster because the return type must be 
    // independent of T.
    return pr::make_any_blind_raster(result_typed);
  }
};

int main()
{
  // prepare a raster in separate scope
  {
    auto raster = pr::create<int>("demo.tif", 3, 4, GDT_Byte);
    auto i = 0;
    for (auto&& v : raster) {
      i = (i + 3) % 7;
      v = i;
    }
  }

  // Open without specifying the type of the raster
  pr::any_blind_raster ras = pr::open_any("demo.tif");
  pr::any_blind_raster ras_sq = pr::blind_function(square{}, ras);
  plot_raster(ras);
  plot_raster(ras_sq);
  return 0;
}