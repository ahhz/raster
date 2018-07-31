//
//=======================================================================
// Copyright 2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
#pragma once

#include <pronto/raster/index_sequence.h>
#include <pronto/raster/traits.h>
#include <pronto/raster/reference_proxy_vector.h>

#include <iterator>
#include <utility>

namespace pronto {
  namespace raster {
    
    template<class> class vector_of_raster_view;

    template<class Raster, bool IsMutable> 
    class vector_of_raster_iterator
    { 
    public:
      
      using single_iterator = typename std::conditional<IsMutable
        , typename traits<Raster>::iterator
        , typename traits<Raster>::const_iterator>::type;

      using reference = reference_proxy_vector <
        typename std::iterator_traits<single_iterator>::value_type, 
        typename std::iterator_traits<single_iterator>::reference>;

      using value_type = std::vector< 
        typename std::iterator_traits<single_iterator>::value_type>;

      using pointer = void;
      using difference_type = 
        typename std::iterator_traits<single_iterator>::difference_type;

      using iterator_category = std::input_iterator_tag;
      
      vector_of_raster_iterator& operator++()
      {
        for (auto&& i : m_iters) ++i;
        return *this;
      }

      vector_of_raster_iterator& operator+=(const difference_type& n)
      {
        for (auto&& i : m_iters) i += n;
        return *this;
      }
      
      vector_of_raster_iterator operator++(int)
      {
        vector_of_raster_iterator temp(*this);
        ++(*this);
        return temp;
      }
     
      vector_of_raster_iterator operator+(const difference_type& n) const
      {
        vector_of_raster_iterator temp(*this);
        temp += n;
        return temp;
      }

      inline reference operator[](std::ptrdiff_t distance) const
      {
        return *(operator+(distance));
      }

      vector_of_raster_iterator& operator--()
      {
        for (auto&& i : m_iters) --i
        return *this;
      }

      vector_of_raster_iterator operator--(int)
      {
        tuple_raster_iterator temp(*this);
        --(*this);
        return temp;
      }

      vector_of_raster_iterator operator-(const difference_type& n) const
      {
        tuple_raster_iterator temp(*this);
        temp -= n;
        return temp;
      }

      vector_of_raster_iterator& operator-=(const difference_type& n)
      {
        for (auto&& i : m_iters) i -= n;
        return *this;
      }

      reference operator*() const
      {
        reference r(m_iters.size());
        auto i = m_iters.begin();
        auto j = r.begin();
        for (;i != m_iters.end(); ++i, ++j)
        {
          *j = **i;
        }
        return r;
      }

      bool operator==(const vector_of_raster_iterator& b) const
      {
        return  m_iters.empty() || m_iters.front() == b.m_iters.front();
      }

      bool operator!=(const vector_of_raster_iterator& b) const
      {
        return  !m_iters.empty() && m_iters.front() != b.m_iters.front();
      }

      bool operator<(const vector_of_raster_iterator& b) const
      {
        return !m_iters.empty() && m_iters.front() < b.m_iters.front();
      }

      bool operator>(const vector_of_raster_iterator& b) const
      {
        return  !m_iters.empty() && m_iters.front() > b.m_iters.front();
      }

      bool operator<=(const vector_of_raster_iterator& b) const
      {
        return  !m_iters.empty() && m_iters.front() <= b.m_iters.front();
      }

      bool operator>=(const vector_of_raster_iterator& b) const
      {
        return !m_iters.empty() != 0 && m_iters.front() > b.m_iters.front();
      }
      
    private:
      friend class vector_of_raster_view<Raster>;
      std::vector<single_iterator> m_iters;
    };

    template<class Raster> // requires the RasterView concepts 
    class vector_of_raster_view
    {
    public:
      using sub_raster_type 
        = vector_of_raster_view<typename traits<Raster>::sub_raster>;
   
      vector_of_raster_view() = default;

      vector_of_raster_view(const std::vector<Raster>& rasters)
        : m_rasters(rasters)
      { }

      using iterator = vector_of_raster_iterator<Raster, true>;
      using const_iterator = vector_of_raster_iterator<Raster, false>;
      
      iterator begin() 
      {
        iterator i;
        for (auto&& r : m_rasters) {
          i.m_iters.emplace_back(r.begin());
        }
        return i;
      }

      iterator end() 
      {
        iterator i;
        for (auto&& r : m_rasters) {
          i.m_iters.emplace_back(r.end());
        }
        return i;
      }

      const_iterator begin() const
      {
        const_iterator i;
        for (auto&& r : m_rasters) {
          i.m_iters.emplace_back(r.begin());
        }
        return i;
      }

      const_iterator end() const
      {
        const_iterator i;
        for (auto&& r : m_rasters) {
          i.m_iters.emplace_back(r.end());
        }
        return i;
      }

      int rows() const
      {
        if (m_rasters.empty()) return 0;
        return m_rasters.front().rows();
      }

      int cols() const
      {
        if (m_rasters.empty()) return 0;
        return m_rasters.front().cols();
      }

      int size() const
      {
        if (m_rasters.empty()) return 0;
        return m_rasters.front().size();
      }

      sub_raster_type sub_raster(int start_row, int start_col, 
        int rows, int cols) const
      {
        using single_subraster = typename traits<Raster>::sub_raster;
        std::vector<single_subraster> subs;
        for (auto&& r : m_rasters) {
          subs.emplace_back(r.sub_raster(start_row, start_col, rows, cols));
        }
        return sub_raster_type(subs);
      }

    private:
      std::vector<Raster> m_rasters;
    };

    template<class Raster> // requires these to be RasterViews
    vector_of_raster_view<Raster> raster_vector(std::vector<Raster>& rasters)
    {
      return vector_of_raster_view<Raster>(rasters);
    }
  }
}
