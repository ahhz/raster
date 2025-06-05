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
#include <pronto/raster/iterator_facade.h>
#include <pronto/raster/reference_proxy.h>

#include <algorithm> // std::min
#include <iterator> // std::ptrdiff_t

namespace pronto
{
  namespace raster
  {
    template<class, iteration_type, access> class gdal_raster_view; // forward declaration

    template<class T, iteration_type IterationType, access AccessType>
    class gdal_raster_iterator
      : public iterator_facade<gdal_raster_iterator<T, IterationType, AccessType>>
    {
      using view_type = gdal_raster_view<T, IterationType, AccessType>;
      using block_type = view_type::block_type;
      using block_iterator_type = typename block_type::iterator;
   
    public:
      static const bool is_single_pass = IterationType==iteration_type::single_pass;
      static const bool is_mutable = AccessType != access::read_only;
      using value_type = T;

      gdal_raster_iterator()
        : m_block()
        , m_end_of_stretch(block_iterator_type()) 
        , m_block_iterator(block_iterator_type())            
        , m_view(nullptr)
      {}

      gdal_raster_iterator(const gdal_raster_iterator& )            = default;
      gdal_raster_iterator(gdal_raster_iterator&& )                 = default;
      gdal_raster_iterator& operator=(const gdal_raster_iterator& ) = default;
      gdal_raster_iterator& operator=(gdal_raster_iterator&& )      = default;
      ~gdal_raster_iterator()                                       = default;
      
      auto dereference() const {
        return *m_block_iterator;
      }

      void increment() {
        m_block_iterator++;
        if (m_block_iterator == m_end_of_stretch) {
          m_block_iterator--;
          goto_index(get_index() + 1);
        }
      }
      void decrement() {
        if (m_end_of_stretch - m_block_iterator < m_view->get_block_cols()) {
          m_block_iterator--;
        }
        else {
            goto_index(get_index() - 1);
        }
      }

      void advance(std::ptrdiff_t offset) {
          goto_index(get_index() + offset);
       }

      bool equal_to(const gdal_raster_iterator& other) const {
        // it is sufficient to compare the block iterators, because
        // there will not be two blocks for the same part of the raster.  
        return m_block_iterator == other.m_block_iterator;
      }

      std::ptrdiff_t distance_to(const gdal_raster_iterator& other) const {
          return other.get_index() - get_index();
      }

    private:
      friend class gdal_raster_view<T, IterationType, AccessType>;

      void find_begin(const view_type* view) {
        m_view = view;
        goto_index(0);
      }

      void find_end(const view_type* view) {
        m_view = view;
        goto_index(static_cast<long long>(m_view->rows()) * static_cast<long long>(m_view->cols()));
      }

    private:
   
      long long get_index() const {
        // it might seem more efficient to just add an index member to the
        // iterator, however the hot-path is operator++ and operator*(),
        // keep those as simple as possible

        int block_rows = m_view->get_block_rows();
        int block_cols = m_view->get_block_cols();

        int index_in_block = m_block_iterator - m_block.begin();

        int minor_row = index_in_block / block_cols;
        int minor_col = index_in_block % block_cols;
        
        int major_row = m_block.major_row();
        int major_col = m_block.major_col();

        int gdaldata_row = major_row * block_rows + minor_row;
        int gdaldata_col = major_col * block_cols + minor_col;

        int row = gdaldata_row - m_view->get_first_row();
        int col = gdaldata_col - m_view->get_first_col();

        // because of the way that the data is organised in blocks it 
        // could be that the "one-past-the-last" iterator points either 
        // to a pixel in one-past-the-last column or one-past-the-last row
        // In either case, the corresponding index is the size of the view
        // 

        if (row == m_view->rows() || col == m_view->cols()) {

          return static_cast<long long>(m_view->rows())
                * static_cast<long long>(m_view->cols() );
        } else {
          return  static_cast<long long>(row) 
              * static_cast<long long> (m_view->cols()) 
              + static_cast<long long>(col);
        }
      }

      void goto_index(long long index)
      {
        if (index == static_cast<long long>(m_view->cols()) * static_cast<long long>(m_view->rows())) {
          if (index == 0) { 
            // empty raster, no place to go
            m_block_iterator = block_iterator_type();
          }
          else {
            // Go to last block, one past the last element.
            // This avoid havings to work out the complexity of where the 
            // end iteratore sits due to the layout in blocks
            goto_index(index - 1);
            m_block_iterator++;
          }
        }
        else {

          int row = static_cast<int>(index / m_view->cols());
          int col = static_cast<int>(index % m_view->cols());

          int gdaldata_row = row + m_view->get_first_row();
          int gdaldata_col = col + m_view->get_first_col();

          int block_rows = m_view->get_block_rows();
          int block_cols = m_view->get_block_cols();

          int major_row = gdaldata_row / block_rows;
          int major_col = gdaldata_col / block_cols;

          int row_in_block = gdaldata_row % block_rows;
          int col_in_block = gdaldata_col % block_cols;

          int index_in_block = row_in_block * block_cols + col_in_block;

          m_view->reset_block(m_block, major_row, major_col);

          m_block_iterator = m_block.begin() + index_in_block;
          
          int first_col_of_next_block = (major_col + 1) * block_cols;
          int end_col_in_dataset = m_view->get_first_col() + m_view->cols();
          int end_col = std::min<int>(first_col_of_next_block, end_col_in_dataset);
         
          m_end_of_stretch = m_block_iterator + (end_col - gdaldata_col);
        }
      }
      const view_type* m_view;
      block_type m_block;
      block_iterator_type m_end_of_stretch;
      block_iterator_type m_block_iterator;
    };
  }
}
