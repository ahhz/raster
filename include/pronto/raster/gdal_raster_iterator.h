//=======================================================================
// Copyright 2015-2022
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once

#include <pronto/raster/access_type.h>
#include <pronto/raster/gdal_block.h>
#include <pronto/raster/gdal_includes.h>
#include <pronto/raster/iterator_facade.h>
#include <pronto/raster/reference_proxy.h>

#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>

namespace pronto
{
  namespace raster
  {
    class single_pass {};
    class multi_pass{};

    template<class, class> class gdal_raster_view; // forward declaration

    template<class T, class AccessType, class IterationType = multi_pass>
    class gdal_raster_iterator 
      : public iterator_facade<gdal_raster_iterator<T,AccessType, IterationType>>
    {
      using block_type = block<AccessType>;
      using block_iterator_type = typename block_type::iterator;
      using view_type = gdal_raster_view<T, IterationType>;

   
    public:
      static const bool is_single_pass = std::is_same<single_pass, IterationType>::value;
      static const bool is_mutable = AccessType::value == access::read_write;
      using value_type = T;

      gdal_raster_iterator()
        : m_block()
        , m_end_of_stretch(m_block.get_null_iterator()) // not so elegant
        , m_pos(m_block.get_null_iterator())            // not so elegant
        , m_stride(0)
      {}

      gdal_raster_iterator(const gdal_raster_iterator& )            = default;
      gdal_raster_iterator(gdal_raster_iterator&& )                 = default;
      gdal_raster_iterator& operator=(const gdal_raster_iterator& ) = default;
      gdal_raster_iterator& operator=(gdal_raster_iterator&& )      = default;
      ~gdal_raster_iterator()                                       = default;

      
      auto dereference() const {
        if constexpr (is_mutable)
        {
        if constexpr (is_single_pass)
        {
          return reference_proxy<const gdal_raster_iterator&>(*this);
        } else {
          return reference_proxy<gdal_raster_iterator>(*this);
        }
        }
        else {
            return m_view->get(static_cast<void*>(m_pos));
        }
      }

      void increment() {
        m_pos += m_stride;

        if (m_pos == m_end_of_stretch) {
            m_pos -= m_stride;
            goto_index(get_index() + 1);
        }
      }
      void decrement() {
        auto d = std::distance(m_block.get_iterator(0, 0, m_view->m_stride), m_pos);
        if (d % m_block.block_cols() > 0) {
            m_pos -= m_stride;
        }
        else
        {
            goto_index(get_index() - 1);
        }
      }

      void advance(std::ptrdiff_t offset) {
          goto_index(get_index() + offset);
       }

      bool equal_to(const gdal_raster_iterator& other) const {
          return m_pos == other.m_pos;
      }

      std::ptrdiff_t distance_to(const gdal_raster_iterator& other) const {
          return other.get_index() - get_index();
      }
    private:
      friend class reference_proxy<const gdal_raster_iterator&>;
      friend class reference_proxy<gdal_raster_iterator>;

      T get() const
      {
        return m_view->get(static_cast<void*>(m_pos));
      }

      void put(const T& value) const
      {
        static_assert(AccessType::value == access::read_write
          , "only allow writing in mutable iterators");
         m_view->put(value, static_cast<void*>(m_pos));
      }

    private:
      friend class gdal_raster_view<T, IterationType>;

      void find_begin(const view_type* view)
      {
        m_view = view;
        m_stride = view->m_stride;
        goto_index(0);
      }

      void find_end(const view_type* view)
      {
        m_view = view;
        m_stride = view->m_stride;
        goto_index(static_cast<long long>(m_view->rows()) * static_cast<long long>(m_view->cols()));
      }

    private:
   
      long long get_index() const
      {
        // it might seem more efficient to just add an index member to the
        // iterator, however the hot-path is operator++ and operator*(),
        // keep those as simple as possible

        int block_rows = m_block.block_rows();
        int block_cols = m_block.block_cols();

        int major_row = m_block.major_row();
        int major_col = m_block.major_col();

        block_iterator_type start = m_block.get_iterator(0, 0, m_view->m_stride);

        int index_in_block = static_cast<int>(std::distance(start, m_pos))
          / m_view->m_stride; //  not -1 because m_pos has not been incremented
        assert(index_in_block >= 0);

        int minor_row = index_in_block / block_cols;
        int minor_col = index_in_block % block_cols;

        int gdaldata_row = major_row * block_rows + minor_row;
        int gdaldata_col = major_col * block_cols + minor_col;
        int row = gdaldata_row - m_view->m_first_row;
        int col = gdaldata_col - m_view->m_first_col;

        long long index;
        // in last block? one past the last element?
        if (row == m_view->rows() || col == m_view->cols()) {

          index =  static_cast<long long>(m_view->rows() )* static_cast<long long>(m_view->cols() );
        } else {
          index =  static_cast<long long>(row) * static_cast<long long> (m_view->cols()) + static_cast<long long>(col);
        }
        assert(index <= static_cast<long long>(m_view->rows()) * static_cast<long long>(m_view->cols()));
        return index;
      }

      void goto_index(long long index)
      {
        if (index == static_cast<long long>(m_view->cols()) * static_cast<long long>(m_view->rows())) {
            if (index == 0) { // empty raster, no place to go
                m_pos = m_block.get_null_iterator();
            }
            else {
                // Go to last block, one past the last element.
                goto_index(index - 1);
                m_pos += m_view->m_stride;
            }
        }
        else {

        int row = static_cast<int>(index / m_view->cols());
        int col = static_cast<int>(index % m_view->cols());

        int gdaldata_row = row + m_view->m_first_row;
        int gdaldata_col = col + m_view->m_first_col;

        int block_rows = 0;
        int block_cols = 0;

        m_view->m_band->GetBlockSize(&block_cols, &block_rows);

        int block_row = gdaldata_row / block_rows;
        int block_col = gdaldata_col / block_cols;

        int row_in_block = gdaldata_row % block_rows;
        int col_in_block = gdaldata_col % block_cols;

        m_block.reset(m_view->m_band.get(), block_row, block_col);
        if constexpr (is_mutable) {
            if (m_view->m_band->GetAccess() == GA_Update) {
                m_block.mark_dirty();
            }
        }
             
        m_pos = m_block.get_iterator(row_in_block, col_in_block, m_view->m_stride);

        int end_col = std::min<int>((block_col + 1) * block_cols
            , m_view->m_first_col + m_view->m_cols);

        int minor_end_col = 1 + (end_col - 1) % block_cols;

        m_end_of_stretch = m_block.get_iterator(row_in_block, minor_end_col,
            m_view->m_stride);
        }
      }

      const view_type* m_view;
      unsigned char m_stride;
      block_type m_block;
      block_iterator_type m_end_of_stretch;
      block_iterator_type m_pos;
    };
  }
}
