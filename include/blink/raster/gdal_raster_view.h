//
//=======================================================================
// Copyright 2015
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// This header file provides the default raster view for iterators that are 
// initialized by the raster and implement the find_begin and find_end functions


#ifndef BLINK_RASTER_GDAL_RASTER_VIEW_H_AHZ
#define BLINK_RASTER_GDAL_RASTER_VIEW_H_AHZ

#include <blink/raster/default_raster_view.h>
#include <blink/raster/gdal_raster.h>
#include <blink/raster/gdal_raster_iterator.h>
#include <blink/raster/raster_iterator.h>
#include <blink/raster/raster_view.h>


namespace blink {
  namespace raster {
    template<class Raster>
    using gdal_raster_view = default_raster_view<Raster, gdal_iterator>;
    //class gdal_raster_view : public default_raster_view<Raster, gdal_iterator>
    //{
    //public:
    //  gdal_raster_view(Raster* r = std::nullptr_t) : default_raster_view(r)
    //  {}
    //};

    template<class Raster>
    class gdal_trans_raster_view : public default_raster_view<Raster,
      gdal_trans_iterator>
    {
    public:
      gdal_trans_raster_view(Raster* r = nullptr) 
        : default_raster_view<Raster, gdal_trans_iterator>(r)
      {}

      index_type size1() const
      {
        return m_raster->size2(); // transposed
      }

      index_type size2() const
      {
        return m_raster->size1(); // transposed
      }
    };

    //template <typename OrientationTag, typename ElementTag, typename AccessTag, typename RasterType>
   // struct raster_view_lookup;

    template <class T>
    struct raster_view_lookup< orientation::row_major, element::pixel,
      access::read_write, gdal_raster<T> >
    {
      using type = gdal_raster_view<gdal_raster<T> >;
    };

    template <class T>
    struct raster_view_lookup< orientation::col_major, element::pixel,
      access::read_write, gdal_raster<T> >
    {
      using type =  gdal_trans_raster_view<gdal_raster<T> >;
    };

    template <class T>
    struct raster_view_lookup< orientation::row_major, element::pixel,
      access::read_only, gdal_raster<T> >
    {
      using type = gdal_raster_view<const gdal_raster<T> >;
    };

    template <class T>
    struct raster_view_lookup< orientation::col_major, element::pixel,
      access::read_only, gdal_raster<T> >
    {
      typedef gdal_trans_raster_view<const gdal_raster<T> > type;
    };
  
    template <class T>
    struct raster_view_lookup< orientation::row_major, element::pixel,
      access::read_write, const gdal_raster<T> >
    {
      using type = gdal_raster_view<const gdal_raster<T> >;
    };

    template <class T>
    struct raster_view_lookup< orientation::col_major, element::pixel,
      access::read_write, const gdal_raster<T> >
    {
      using type = gdal_trans_raster_view<const gdal_raster<T> >;
    };

    template <class T>
    struct raster_view_lookup< orientation::row_major, element::pixel,
      access::read_only, const gdal_raster<T> >
    {
      using type = gdal_raster_view<const gdal_raster<T> >;
    };

    template <class T>
    struct raster_view_lookup< orientation::col_major, element::pixel,
      access::read_only, const gdal_raster<T> >
    {
      typedef gdal_trans_raster_view<const gdal_raster<T> > type;
    };
  }
}
#endif