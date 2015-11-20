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

  }
} 
#endif