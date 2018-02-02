//example_blind_function.cpp

#include <pronto/raster/any_blind_raster.h>
#include <pronto/raster/blind_function.h>
#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/transform_raster_view.h>

namespace br = pronto::raster;

struct square
{
public:
  template<class T>
  br::any_blind_raster operator()(const br::any_raster<T>& raster) const
  {
    auto sq_fun = [](const T& v) {return v * v; };
    auto result_typed = br::transform(sq_fun, raster);

    // wrapping as any_blind_raster because the return type must be 
    // independent of T.
    return br::make_any_blind_raster(result_typed);
  }
};

int main()
{
  // prepare a raster in separate scope
  {
    auto raster = br::create<int>("demo.tif", 3, 4, GDT_Byte);
    auto i = 0;
    for (auto&& v : raster) {
      i = (i + 3) % 7;
      v = i;
    }
  }

  // Open without specifying the type of the raster
  br::any_blind_raster ras = br::open_any("demo.tif");
  br::any_blind_raster ras_sq = br::blind_function(square{}, ras);
  plot_raster(ras);
  plot_raster(ras_sq);
  return 0;
}