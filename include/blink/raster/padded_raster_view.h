//
//=======================================================================
// Copyright 2016
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#ifndef BLINK_RASTER_PADDED_RASTER_VIEW_H_AHZ
#define BLINK_RASTER_PADDED_RASTER_VIEW_H_AHZ

#include <blink/raster/dereference_proxy.h>
#include <blink/iterator/detail.h>
#include <boost/optional.hpp>

//#include <utility>
#include <cassert>
#include <memory>
#include <type_traits>

namespace blink {
  namespace raster {
    //// Forward declaration for friendship;
     template<class Raster> class padded_raster_view;

    template<class Raster, class T = typename std::decay<Raster>::type::iterator::value_type >
    class padded_raster_iterator
      : public std::iterator<std::input_iterator_tag, boost::optional<T>
      , dereference_proxy<padded_raster_iterator<Raster,T>,boost::optional<T> > >
    {
    public:
      using reference = dereference_proxy<padded_raster_iterator, boost::optional<T> >;

      padded_raster_iterator() : m_view(nullptr)
      {};

      padded_raster_iterator(const padded_raster_iterator& other)
        = default;
      //padded_raster_iterator(padded_raster_iterator&& other) = default;
      padded_raster_iterator& operator=(
        const padded_raster_iterator& other) = default;
      //padded_raster_iterator& operator=(
      //  padded_raster_iterator&& other) = default;
      ~padded_raster_iterator() = default;

      padded_raster_iterator& operator++()
      {
        while (m_remaining == 0) {
          ++m_stretch;
          if (m_stretch == m_view->m_num_stretches) {
            return *this;
          }

          m_remaining = m_view->stretch_length(m_stretch);
          m_is_padding = !m_is_padding;
          if (m_remaining)
          {
            m_remaining--;
            if (!m_is_padding && m_stretch != 1){
              ++m_iter;
            }

            return *this;
          }
        }
        --m_remaining;
        if (!m_is_padding){
          ++m_iter;
        }
        return *this;
      }

      padded_raster_iterator operator++(int)
      {
        padded_raster_iterator temp(*this);
        ++(*this);
        return temp;
      }

      reference operator*() const
      {
        return reference(this);
      }

      boost::optional<T> get() const
      {
        if (m_is_padding) return boost::none;
        else return boost::optional<T>(*m_iter);
      }

      // TODO: Would it be better to make this a non_mutable input iterator
      // and not allow putting values?
      void put(const boost::optional<T>& value) const
      {
        if (value && !m_is_padding) {
          *m_iter = *value;
        }
      }

    public:
      friend bool operator==(const padded_raster_iterator& a,
        const padded_raster_iterator& b)
      {
        return a.m_stretch == b.m_stretch && a.m_remaining == b.m_remaining;
      }

      friend bool operator!=(const padded_raster_iterator& a,
        const padded_raster_iterator& b)
      {
        return a.m_stretch != b.m_stretch || a.m_remaining != b.m_remaining;
      }

    private:

      template<typename U>
      friend class padded_raster_view;

      void find_begin(padded_raster_view<Raster>* view) 
      {
        m_view = view;
        m_iter = m_view->m_raster.begin();
        m_stretch = 0;
        m_remaining = m_view->stretch_length(0) -1;
        m_is_padding = true;
      }

      void find_end(padded_raster_view<Raster>* view)
      {
        m_view = view;
        m_iter = m_view->m_raster.end();
        m_stretch = m_view->m_num_stretches;
        m_remaining = 0;
        m_is_padding = false;
      }

      void update()
      {
        m_is_padding = ~m_is_padding;
        ++m_stretch;
        m_remaining_in_stretch = m_view->get_stretch_length(m_stretch);
      }

    private:
      int m_stretch;
      int m_remaining;
      bool m_is_padding;
      typename std::decay<Raster>::type::iterator m_iter;
      padded_raster_view<Raster>* m_view;
    };
   

    template<typename Raster> // raster may be a reference
    class padded_raster_view
    {
    public:
      using iterator = padded_raster_iterator < Raster > ;
      using const_iterator = padded_raster_iterator < Raster >;

      padded_raster_view() :m_raster(nullptr)
      {}


      template<class InRaster>
      padded_raster_view(InRaster&& raster, 
        int row_before, int row_after, 
        int col_before, int col_after) 
        : m_raster(std::forward<InRaster>(raster))
      {
        set(m_raster.rows(), m_raster.cols(), row_before, row_after, col_before, col_after);
      }

      iterator begin() 
      {
        iterator i;
        i.find_begin(this);
        return i;
      }

      iterator end() 
      {
        iterator i;
        i.find_end(this);
        return i;
      }

      const_iterator begin() const
      {
        const_iterator i;
        i.find_begin(this);
        return i;
      }

      const_iterator end() const
      {
        const_iterator i;
        i.find_end(this);
        return i;
      }

      const_iterator cbegin() const
      {
        const_iterator i;
        i.find_begin(this);
        return i;
      }

      const_iterator cend() const
      {
        const_iterator i;
        i.find_end(this);
        return i;
      }

    private:
      friend class padded_raster_iterator < Raster > ;
      void set(int rows, int cols, int pad_row_before, int pad_row_after
        , int pad_col_before, int pad_col_after)
      {
        m_num_stretches = rows * 2 + 1;
        m_first =  (cols + pad_col_before + pad_col_after) * pad_row_before 
          + pad_col_before;
        m_last  = (cols + pad_col_before + pad_col_after) * pad_row_after +
          pad_col_after;
        m_even = pad_col_before + pad_col_after;
        m_odd = cols;
        m_padding_is_odd = false;
      }
      
      int stretch_length(int stretch_index)
      {
        if (stretch_index == 0)
        {
          return m_first;
        }
        else if (stretch_index == m_num_stretches - 1)
        {
          return m_last;
        }
        else if (stretch_index % 2 == 0) {
          return m_even;
        }
        else
        {
          return m_odd;
        }
      }

      bool is_padding(int stretch_index)
      {
        const bool is_odd = (stretch_index % 2) == 1;
        return (is_odd == m_padding_is_odd);
      }

      Raster m_raster;
      int m_num_stretches;
      int m_last;
      int m_first;
      int m_even;
      int m_odd;
      bool m_padding_is_odd;

    };
   
    template<class Raster>
    padded_raster_view<blink::iterator::detail::special_decay_t<Raster> > pad_raster(Raster&& raster
      , int row_before, int row_after, int col_before, int col_after)
    {
      return padded_raster_view<blink::iterator::detail::special_decay_t<Raster> >(std::forward<Raster>(raster)
        , row_before, row_after, col_before, col_after);
    }
  }
}

#endif


