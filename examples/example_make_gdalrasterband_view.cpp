//example_make_gdalrasterdata_view.cpp

#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>

namespace br = pronto::raster;

int main()
{
  // prepare a file in a separate scope 
  {
    auto raster = br::create<int>("test.tif", 3, 4, GDT_Byte);
    auto i = 0;
    for (auto&& v : raster) {
      i = (i + 3) % 7;
      v = i;
    }
  }
  // open dataset separately from band 
  std::shared_ptr<GDALDataset> dataset = br::detail::open_dataset("test.tif", br::read_only);
  std::shared_ptr<GDALRasterBand> band  = br::detail::open_band(dataset, 1);

  // Note that here the type must be specified
  auto view = br::make_gdalrasterdata_view<int>(band);

  plot_raster(view);
  return 0;
}