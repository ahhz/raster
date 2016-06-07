//
//=======================================================================
// Copyright 2015
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// This file provides a Least Recent Used cache of data blocks for gdal_datasets 
// When additional memory is required, beyond the memory budget, then the block
// in the matrix that is *least recently used* will be cleared.

// This is providing functionality that is also present in GDAL. It should be 
// investigated why GDAL appeared too slow. 
//
// By using this LRU space, only the most basic interface in GDAL (ReadBlock 
// and WriteBlock) needs to be used. And not the friendlier and GDAL-recommended
// RasterIO. 

#ifndef BLINK_RASTER_GDAL_RASTER_LRU_H_AHZ
#define BLINK_RASTER_GDAL_RASTER_LRU_H_AHZ

#include <blink/raster/exceptions.h>

#include <boost/optional.hpp>

#include <cstddef>
#include <cassert> 
#include <list>
#include <memory>
#include <vector>


class GDALRasterBand;

namespace blink {
  namespace raster {
    namespace LRU
    {
      class block
      {
      public:
        typedef std::list<block>::iterator  block_list_iterator;
        typedef boost::optional<block_list_iterator> optional_block_list_iterator;

        block(int row, int col, std::size_t size, optional_block_list_iterator& vector_element, GDALRasterBand* band);
        ~block();
        void mark_dirty();
        std::size_t get_size() const;
        void* get_buffer();
        const void* get_buffer() const;

      private:
        int m_major_row;
        int m_major_col;
        GDALRasterBand* m_gdal_rasterband;
        optional_block_list_iterator& m_vector_element;
        bool m_is_dirty;
        std::vector<char> m_buffer;
      };

      class block_cache
      {
      public:
        //! Use the constructor to reserve memory capacity
        //! \param capacity the amount of memory to reserve in bytes
        block_cache(size_t capacity = 2e8);
        block_cache(block_cache&& that);
        block_cache& operator=(block_cache&& that);

      private:
        friend class block_manager;
        typedef std::list<block>::iterator  block_list_iterator;
        typedef boost::optional<block_list_iterator> optional_block_list_iterator;

        //! moves the block that the iter points to the back of the queue
        void touch(block_list_iterator& iter);
        //! commits the block to disk and removes from cache
        void remove(block_list_iterator& iter);

        //! removes blocks from cache until there is space for this new one. Then 
        //! makes it. 
        //! \param row major row index
        //! \param col major col index
        //! \param size sizeof the block in bytes
        //! \param vector_element this is where the block_manager keeps the iterator 
        //! to the cache element
        void make(
          size_t row,
          size_t col,
          size_t size,
          optional_block_list_iterator& vector_element, // may be boost::none 
          GDALRasterBand* band);

        std::list<block> m_cache;
        size_t m_size;
        size_t m_capacity;
      };

      extern block_cache g_block_cache;

      //! The block manager keeps track to see which blocks of a GDALRasterBand are 
      //! in the cache, 
      //! and if so, where they are.
     class block_manager
      {
        typedef std::list<block>::iterator  block_list_iterator;
        typedef boost::optional<block_list_iterator> optional_block_list_iterator;

      public:
        //! The constructor
        //! \param band the pointer to the band that will be accessed through this 
        //! iterator
        //! \param cache the cache in which block data will be held.By default this
        //! is the global cache
        block_manager(GDALRasterBand* band, block_cache* cache = nullptr);

        block_manager();

        block_manager(block_manager&& that);
        ~block_manager();

        block_manager& operator=(block_manager&& that);

        void flush();

        //! Get a writable block by its block index
        //!
        void* get_for_writing(size_t index);
        //! Get a read only block by its block index
        //!
        const void* get(size_t index) const;
      private:
        block_list_iterator get_list_iterator(size_t index) const;


        mutable std::vector<optional_block_list_iterator> m_index;
        block_cache* m_block_cache;
        GDALRasterBand* m_gdal_rasterband;
        size_t m_block_size;
        size_t m_major_size1;
        size_t m_major_size2;
      };

      //! The global block_cache. This is a block cache with default size that can
      //! be used as a default when none is provided.
    }; //LRU namespace
  }
}
#endif