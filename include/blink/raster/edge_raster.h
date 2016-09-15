//
//=======================================================================
// Copyright 2016
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
// Intended to replace edge_view and edge_iterator (creative destruction)

#ifndef BLINK_RASTER_EDGE_RASTER_H_AHZ
#define BLINK_RASTER_EDGE_RASTER_H_AHZ

#include <blink/raster/pad_raster.h>
#include <blink/iterator/zip_pair_range.h>
#include <blink/iterator/zip_range.h>
#include <blink/iterator/detail.h>

namespace blink {
  namespace raster {
    namespace detail {


      template<class Raster, bool IsHorizontal>
      class edge_raster_view
      {
        static const int one_if_horizontal = IsHorizontal ? 1 : 0;
        static const int one_if_vertical = IsHorizontal ? 0 : 1;

        using raster_pair = blink::iterator::zip_pair_range<
          padded_raster_view<Raster&>, padded_raster_view<Raster&> >;
      public:
        using iterator = typename raster_pair::iterator;

        template<class InRaster>
        edge_raster_view(InRaster&& raster)
          : m_raster(std::forward<InRaster>(raster))
          , m_raster_pair
            ( pad_raster(std::ref(m_raster), one_if_horizontal, 0, one_if_vertical, 0)
            , pad_raster(std::ref(m_raster), 0, one_if_horizontal, 0, one_if_vertical))
        {}

        iterator begin()
        {
          return m_raster_pair.begin();
        }

        iterator end()
        {
          return m_raster_pair.end();
        }

        int rows()
        {
          return m_raster.rows() + one_if_horizontal;
        }

        int cols()
        {
          return m_raster.cols() + one_if_vertical;
        }

        Raster m_raster;           // order of declaration matters: first
        raster_pair m_raster_pair; // order of declaration matters: second
      };
    }

    template<class Raster>
    using h_edge_raster_view = detail::edge_raster_view<Raster, true>;

    template<class Raster>
    using v_edge_raster_view = detail::edge_raster_view<Raster, false>;

    using blink::iterator::special_decay_t;
    
    template<class Raster>
    h_edge_raster_view<special_decay_t<Raster> > h_edge_raster(Raster&& raster)
    {
      return h_edge_raster_view<special_decay_t<Raster> >
        (std::forward<Raster>(raster));
    }

    template<class Raster>
    v_edge_raster_view<special_decay_t<Raster> > v_edge_raster(Raster&& raster)
     {
      return v_edge_raster_view<special_decay_t<Raster> >
        (std::forward<Raster>(raster));
    }
    
   
  }
}

#endif


