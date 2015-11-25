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

void print_value(const boost::none_t&)
{
  std::cout << 'x';
}


template<class Value>
void print_value(const Value& v)
{
  std::cout << v;
}

template<class Value>
void print_value(const boost::optional<Value>& v)
{
  if (v){  
    print_value(*v);
  }
  else
  {
    print_value(boost::none);
  }
}
template<class A, class B>
void print_value(const std::pair<A, B>& v)
{
  print_value(v.first);
  std::cout << ' ';
  print_value(v.second);
}


template<class Iterator, class Value>
void print_value(const blink::raster::dereference_proxy<Iterator, Value>& value)
{
  auto v = Value{ value };
  print_value(v);
}

int main()
{
  //create_small();
  //return 0;

  auto input = blink::raster::open_gdal_raster<int>("small.tif", GA_Update);
   
  auto orientation = blink::raster::orientation::col_major{};
  auto access = blink::raster::access::read_write{};
  auto element = blink::raster::element::v_edge{};

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