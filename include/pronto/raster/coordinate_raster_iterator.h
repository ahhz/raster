//=======================================================================
// Copyright 2015-2020
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once


#include <cstddef>
#include <iterator>
#include <utility>

namespace pronto
{
  namespace raster
  {
    template<class> class coordinate_raster_view; // forward declaration
 
    template<class CoordinateType>
    class coordinate_raster_iterator
    {
      using view_type = coordinate_raster_view<CoordinateType>;
    
    public:
      using is_mutable = std::integral_constant<bool, false>;

      using reference = CoordinateType;
      using value_type = CoordinateType;
      using difference_type = std::ptrdiff_t;
      using pointer = void;
      using iterator_category = std::output_iterator_tag;

      coordinate_raster_iterator() = default;
      coordinate_raster_iterator(const coordinate_raster_iterator&) = default;
      coordinate_raster_iterator(coordinate_raster_iterator&&) = default;
      coordinate_raster_iterator& operator=(const coordinate_raster_iterator& )= default;
      coordinate_raster_iterator& operator=(coordinate_raster_iterator&& ) = default;
      ~coordinate_raster_iterator() = default;

      friend bool operator==(const coordinate_raster_iterator& a
        , const coordinate_raster_iterator& b)
      {
        return a.get_index() == b.get_index();
      }

      friend bool operator!=(const coordinate_raster_iterator& a
        , const coordinate_raster_iterator& b)
      {
        return a.get_index() != b.get_index();
      }

      coordinate_raster_iterator& operator+=(std::ptrdiff_t distance)
      {
        goto_index(get_index() + static_cast<long long>(distance));
        return *this;
      }

      coordinate_raster_iterator& operator-=(std::ptrdiff_t distance)
      {
        goto_index(get_index() - static_cast<long long>(distance));
        return *this;
      }

      coordinate_raster_iterator& operator--()
      {
        return goto_index(get_index() - 1);
      }

      coordinate_raster_iterator& operator--(int)
      {
        coordinate_raster_iterator temp(*this);
        --(*this);
        return temp;
      }

      coordinate_raster_iterator operator+(std::ptrdiff_t distance) const
      {
        coordinate_raster_iterator temp(*this);
        temp += distance;
        return temp;
      }

      coordinate_raster_iterator operator-(std::ptrdiff_t distance) const
      {
        coordinate_raster_iterator temp(*this);
        temp -= distance;
        return temp;
      }

      reference operator[](std::ptrdiff_t distance) const
      {
         return *(operator+(distance));
      }

      bool operator<(const coordinate_raster_iterator& that) const
      {
        return get_index() < that.get_index();
      }

      bool operator>(const coordinate_raster_iterator& that) const
      {
        return get_index() > that.get_index();
      }

      bool operator<=(const coordinate_raster_iterator& that) const
      {
        return get_index() <= that.get_index();
      }

      bool operator>=(const coordinate_raster_iterator& that) const
      {
        return get_index() >= that.get_index();
      }

      coordinate_raster_iterator& operator++()
      {
        return goto_index(get_index() + 1);
      }

      coordinate_raster_iterator operator++(int)
      {
        coordinate_raster_iterator temp(*this);
        ++(*this);
        return temp;
      }

       reference operator*() const
      {
        return m_pos;
      }
    private:
      friend class coordinate_raster_view<CoordinateType>;

      void find_begin(const view_type* view)
      {
        m_first = CoordinateType(view->m_first_row, view->m_first_col);
        m_size = CoordinateType(view->rows(), view->cols());
        goto_index(0);
      }

      void find_end(const view_type* view)
      {
         m_first = CoordinateType(view->m_first_row, view->m_first_col);
        m_size = CoordinateType(view->rows(), view->cols());
        goto_index(static_cast<long long>(view->rows()) * static_cast<long long>(view->cols()));
      }

    private:
     
      long long get_index() const
      {
        auto r = static_cast<long long>(m_pos.first - m_first.first);
        auto c = static_cast<long long>(m_pos.second - m_first.second);
        return r * m_size.second + c;
      }
      coordinate_raster_iterator& goto_index(long long index)
      {
        m_pos.first = static_cast<typename CoordinateType::first_type> (
          m_first.first + index / m_size.second);
        m_pos.second = static_cast<typename CoordinateType::second_type> (
          m_first.second + index % m_size.second);
        return *this;
      }
      CoordinateType m_pos;
      CoordinateType m_first;
      CoordinateType m_size;


    };
  }
}
