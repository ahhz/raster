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

#ifndef BLINK_RASTER_DEFAULT_RASTER_VIEW_H_AHZ
#define BLINK_RASTER_DEFAULT_RASTER_VIEW_H_AHZ


namespace blink {
  namespace raster {

    template<class Raster, template<class...> class Iterator, class... IteratorArguments>
    class default_raster_view
    {
    public:
      using coordinate_type = typename Raster::coordinate_type;
      using value_type = typename Raster::value_type;
      using index_type = typename Raster::index_type;
      using iterator = Iterator<typename Raster, IteratorArguments...>;
      using const_iterator = Iterator<const typename Raster, IteratorArguments...>;

      default_raster_view(Raster* r = nullptr) : m_raster(r)
      {}

      iterator begin()
      {
        iterator i(m_raster);
        i.find_begin();
        return i;
      }

      iterator end()
      {
        iterator i(m_raster);
        i.find_end();
        return i;
      }

      const_iterator begin() const
      {
        const_iterator i(m_raster);
        i.find_begin();
        return i;
      }

      const_iterator end() const
      {
        const_iterator i(m_raster);
        i.find_end();
        return i;
      }
      const_iterator cbegin() const
      {
        return begin();
      }

      const_iterator cend() const
      {
        return end();
      }

      index_type size1() const
      {
        return m_raster->size1(); // or size2
      }

      index_type size2() const
      {
        return m_raster->size2(); // or size1
      }

      Raster* m_raster;
    };
  }
}
#endif
