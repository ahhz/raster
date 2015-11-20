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
int main()
{
  auto input = blink::raster::open_gdal_raster<int>("small.tif", GA_Update);

  
  int sum = 0;
  for (auto&& i : input)
  {
    std::cout << (int)i << std::endl;

  }
 
  auto trans = blink::raster::raster_view
    < blink::raster::orientation::col_major
    , blink::raster::element::pixel
    , blink::raster::access::read_write
    , blink::raster::gdal_raster<int> >(&input);

  int sum2 = 0;
  for (auto&& i : trans)
  {
    std::cout << (int)i << std::endl;
    sum2 += i;
  }
  
 /* using view_type = blink::raster::raster_view
    < blink::raster::orientation::col_major
    , blink::raster::element::h_edge
    , blink::raster::access::read_write
    , blink::raster::gdal_raster<int> >;
  using view_type = blink::raster::raster_view_lookup
    < blink::raster::orientation::col_major
    , blink::raster::element::h_edge
    , blink::raster::access::read_write
    , blink::raster::gdal_raster<int> >::type;
  */
 using view_type = blink::raster::detail::edge_view_lookup
    < blink::raster::orientation::col_major
    , blink::raster::element::h_edge
    , blink::raster::access::read_write
    , blink::raster::gdal_raster<int> >::type;


  using view_type2 = blink::raster::h_edge_trans_view<
    blink::raster::gdal_raster<int>,
    blink::raster::get_strategy::both>;

  static_assert (std::is_same<view_type, blink::raster::h_edge_trans_view<
    blink::raster::gdal_raster<int>,
    blink::raster::get_strategy::both> >::value, "check view_type");
 
 
  using iterator_type = view_type2::iterator;
  using value_type = iterator_type::value_type;

  auto h_edge = view_type2(&input);

  int sum4 = 0;
  for (auto&& i : h_edge)
  {
    value_type v = i;

    if (v.first){  // i.second doesn't work because i is a proxy
      std::cout << *v.first;
    }
    else
    {
      std::cout << 'x';
    }
    std::cout << ' ';
    if (v.second){  // i.second doesn't work because i is a proxy
      std::cout << *v.second;
    }
    else
    {
      std::cout << 'x';
    }
    std::cout << std::endl;

  }
 
  return 0;
}