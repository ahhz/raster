//
//=======================================================================
// Copyright 2015
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#ifndef RASTER_VIEW_H_AHZ
#define RASTER_VIEW_H_AHZ

namespace blink {

  namespace raster {

    template<class Orientation, class Element, class Access, class Raster>
    struct raster_view_lookup
    {
      struct this_must_be_specialized;
      using type = this_must_be_specialized;
    };

    template<class Orientation, class Element, class Access, class Raster>
    using raster_view = typename raster_view_lookup<Orientation, Element, Access, 
      Raster>::type;

    template
      < class Orientation = blink::raster::orientation::row_major
      , class Element = blink::raster::element::pixel
      , class Access = blink::raster::access::read_write
      , class Raster>
      raster_view<Orientation, Element, Access, Raster>
      make_raster_view(Raster* raster, 
      const Orientation& orientation = Orientation{},
      const Element& element = Element{},
      const Access& access = Access{})
    {
      return raster_view<Orientation, Element, Access, Raster>(raster);
    }
  }
} 
#endif