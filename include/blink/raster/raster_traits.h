//
//=======================================================================
// Copyright 2015
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// raster_traits are used to inspect properties of rasters
//     - value_type
//     - coordinate_type
//     - index_type
//     - iterator
// raster_operations define free functions to be overloaded for each raster type. 
// that default to appropriate member functions. 

#ifndef BLINK_RASTER_RASTER_TRAITS_H_AHZ
#define BLINK_RASTER_RASTER_TRAITS_H_AHZ

namespace blink {

  namespace raster {

    namespace raster_traits
    {
      template<typename Raster>
      using value_type = typename Raster::value_type;

      template<typename Raster>
      using coordinate_type = typename Raster::coordinate_type;
   
      template<typename Raster>
      using index_type = typename Raster::index_type;

      template<typename Raster>
      using iterator = typename Raster::iterator;

      template<typename Raster>
      using const_iterator = typename Raster::const_iterator;
      /*
      template <typename OrientationTag, typename ElementTag, typename AccessTag, typename RasterType>
      struct iterator
      {
        struct has_no_default_implementation{};
        typedef has_no_default_implementation type;
      };

      */

    };

    namespace raster_operations
    {
      /*
      template <typename OrientationTag, typename ElementTag, typename AccessTag, typename Raster>
      typename raster_traits::iterator< OrientationTag, ElementTag, AccessTag, Raster>
        begin(Raster& r)
      {
          typedef typename raster_traits::iterator< OrientationTag, ElementTag, AccessTag, Raster>::type
            iterator;
          return r.begin<iterator>();
      }
      template <typename OrientationTag, typename ElementTag, typename AccessTag, typename Raster>
      typename raster_traits::iterator< OrientationTag, ElementTag, AccessTag, Raster>
        end(Raster& r)
      {
          typedef typename raster_traits::iterator< OrientationTag, ElementTag, AccessTag, Raster>::type
            iterator;
          return r.end<iterator>();
      }
      */
    }


    namespace raster_operations
    {
      template<typename Raster>
      typename raster_traits::index_type<Raster> size1(const Raster& r)
      {
        return r.size1();
      }

      template<typename Raster>
      typename raster_traits::index_type<Raster>  size2(const Raster& r)
      {
        return r.size2();
      }
    }

    namespace raster_operations
    {
      template<typename Raster>
      typename raster_traits::index_type<Raster> block_size1(const Raster& r)
      {
        return r.block_size1();
      }

      template<typename Raster>
      typename raster_traits::index_type<Raster>  block_size2(const Raster& r)
      {
        return r.block_size2();
      }

      template<typename Raster>
      typename raster_traits::value_type<Raster> get_pixel_in_block(
        const Raster& r,
        typename raster_traits::index_type<Raster> block,
        typename raster_traits::index_type<Raster> pixel_in_block)
      {
        return r.get_pixel_in_block(block, pixel_in_block);
      }

      template<typename Raster>
      void put_pixel_in_block(
        Raster& r,
        const typename raster_traits::value_type<Raster>& value,
        typename raster_traits::index_type<Raster> block,
        typename raster_traits::index_type<Raster> pixel_in_block)
      {
        r.put_pixel_in_block(block, pixel_in_block, value);
      }
    };
  }
} 
#endif