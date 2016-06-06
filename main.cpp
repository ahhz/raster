#include <boost/optional/optional_io.hpp>
#include <blink/iterator/zip_range.h>
#include <blink/raster/edge_iterator.h>
#include <blink/raster/edge_view.h>
#include <blink/raster/gdal_raster_view.h>
#include <blink/raster/raster_iterator.h>
#include <blink/raster/raster_traits.h>
#include <blink/raster/utility.h>

void create_small()
{
  auto output = blink::raster::create_gdal_raster<int>("small.tif", 3, 4);
  int index = 0;
  for (auto&& i : output)
  {
    i = (index++);
  }
}

void create_very_small()
{
  auto output = blink::raster::create_gdal_raster<int>("very_small.tif", 1, 1);
  int index = 0;
  for (auto&& i : output)
  {
    i = (index++);
  }
}

/*
void create_zilch()
{
  auto output = blink::raster::create_gdal_raster<int>("zilch.tif", 0, 0);
  int index = 0;
  for (auto&& i : output)
  {
    i = (index++);
  }
}
*/

template<class Value>
void print_value(const Value& v)
{
  std::cout << v;
}

template<class A, class B>
void print_value(const std::pair<A, B>& v)
{
  std::cout << v.first << ' ' << v.second;
}

template<class I, class T>
void print_value(const blink::raster::dereference_proxy<I, T>& v)
{
  print_value(static_cast<T>(v));
}

int test()
{
  auto a = blink::raster::open_gdal_raster<int>("small.tif", GA_Update);
  auto b = blink::raster::create_temp_gdal_raster_from_model<int>(a);
  auto out = blink::raster::create_gdal_raster_from_model<int>("small_out.tif", a);
  auto zip = blink::iterator::make_zip_range(std::ref(a), std::ref(b),
    std::ref(out));
  for (auto&& i : zip)
  {
    std::get<1>(i) += std::get<0>(i);
    std::get<2>(i) = std::get<0>(i) + std::get<1>(i);
  }

  auto ai = a.begin();
  auto bi = b.begin();
  auto outi = out.begin();
  for (; ai != a.end(); ++ai, ++bi, ++outi)
  {
    *outi = *ai + *bi;
  }

  return 0;
}


int main()
{
  create_small();
  return 0;
  //return test();
  auto input = blink::raster::open_gdal_raster<int>("small.tif", GA_Update);
   
  auto orientation = blink::raster::orientation::col_major{};
  auto access = blink::raster::access::read_write{};
  auto element = blink::raster::element::pixel{};

  auto view = blink::raster::make_raster_view(&input, orientation, element, 
    access);
  for (auto&& i : view)
  {
    print_value(i);
    std::cout << std::endl;
  }
 
  std::cout << std::endl;
   
  return 0;
}