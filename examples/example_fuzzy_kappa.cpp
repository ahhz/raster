//example_fuzzy_kappa.cpp

#include <pronto/raster/fuzzy_kappa.h>
#include <pronto/raster/io.h>
#include <pronto/raster/plot_raster.h>
#include <pronto/raster/uniform_raster_view.h>
#include <pronto/raster/vector_of_raster_view.h>


namespace br = pronto::raster;




int main()
{
  auto a = br::create_temp<int>(5, 5);
  auto b = br::create_temp<int>(5, 5);
  auto result = br::create_temp<double>(5, 5);
  auto mask = br::uniform(5, 5, 1);
  
  auto a_vector = { 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0,
                    0, 0, 1, 0, 0,
                    0, 0, 0, 2, 0,
                    0, 0, 0, 0, 0 };

  auto b_vector = { 0, 0, 0, 0, 0,
                    0, 1, 0, 0, 0,
                    0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0 };
  br::assign(a, a_vector);
  br::assign(b, b_vector);
  
  br::matrix<double> m(3, std::vector<double>(3, 0));
  m[0][0] = 1;
  m[1][1] = 1;
  m[2][2] = 1;

  auto f = br::one_neighbour(0.5);
  double stat;
  auto success = br::fuzzy_kappa_2009(a, b, mask, 3, 3, m, f, result, br::gdal_raster_maker{}, stat);
  br::plot_raster(result);
  return 0;
}