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

#include <blink\raster\exceptions.h>

#include <cpl_conv.h>
#pragma warning( push )
#pragma warning( disable : 4251 )//std::vector needs to have dll-interface 
#include <gdal_priv.h>
#pragma warning( pop )

#include <boost/optional.hpp>

#include <cassert> 
#include <list>
#include <memory>
#include <vector>

namespace blink {
  namespace raster {



    namespace LRU
    {
      class block
      {
      public:
        //friend class block_cache;
        //friend class block_manager;
        //friend class std::allocator<std::_List_node<LRU::block, void *>>; // to allow deletion from list
        typedef std::list<block>::iterator  block_list_iterator;
        typedef boost::optional<block_list_iterator> optional_block_list_iterator;

        block(int row, int col, size_t size, optional_block_list_iterator& vector_element, GDALRasterBand* band)
          : m_major_row(row)
          , m_major_col(col)
          , m_vector_element(vector_element)
          , m_gdal_rasterband(band)
          , m_is_dirty(false)
          , m_buffer(size)
        {
          CPLErr error = m_gdal_rasterband->ReadBlock(m_major_col, m_major_row,
            get_buffer());
          if (error != CE_None) {
            BOOST_THROW_EXCEPTION(reading_from_raster_failed{});
          }
        }
        ~block()
        {
          if (m_is_dirty) {
            if (m_gdal_rasterband->GetAccess() != GA_Update) {
              BOOST_THROW_EXCEPTION(writing_to_raster_failed{});
            };
            CPLErr error = m_gdal_rasterband->WriteBlock(m_major_col, m_major_row,
              get_buffer());
            if (error != CE_None) {
              BOOST_THROW_EXCEPTION(writing_to_raster_failed{});
            }
          }
          //m_is_dirty = false;
          m_vector_element = boost::none;
        }

        void mark_dirty()
        {
          m_is_dirty = true;
        }
        size_t get_size() const
        {
          return m_buffer.size();
        }

        void* get_buffer()
        {
          return static_cast<void*>(&m_buffer[0]);
        }
        const void* get_buffer() const
        {
          return static_cast<const void*>(&m_buffer[0]);
        }

      private:
        int m_major_row;
        int m_major_col;
        GDALRasterBand* m_gdal_rasterband;
        optional_block_list_iterator& m_vector_element;
        bool m_is_dirty;
        std::vector<char> m_buffer;
      };

      //! A block cache is a volume of reserved memory for the block_manager
      //!
      class block_cache
      {
      public:
        //! Use the constructor to reserve memory capacity
        //! \param capacity the amount of memory to reserve in bytes
        block_cache(size_t capacity = 2e8) : m_capacity(capacity), m_size(0)
        {}

        block_cache(block_cache&& that)
          : m_cache(std::move(that.m_cache))
          , m_size(that.m_size)
          , m_capacity(that.m_capacity)
        {}

        block_cache& operator=(block_cache&& that)
        {
          if (this != &that)
          {
            m_cache = std::move(that.m_cache);
            m_size = that.m_size;
            m_capacity = that.m_capacity;
          }
          return *this;
        }

      private:
        friend class block_manager;
        typedef std::list<block>::iterator  block_list_iterator;
        typedef boost::optional<block_list_iterator> optional_block_list_iterator;

        //! moves the block that the iter points to the back of the queue
        void touch(block_list_iterator& iter)
        {
          m_cache.splice(m_cache.end(), m_cache, iter);
        }

        //! commits the block to disk and removes from cache
        void remove(block_list_iterator& iter)
        {
          m_size -= iter->get_size();
          m_cache.erase(iter); // iter is deleted by now
        }

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
          GDALRasterBand* band)
        {
          if (size > m_capacity)  {
            BOOST_THROW_EXCEPTION(insufficient_memory_for_raster_block{});
          }

          m_size += size;
          while (m_size > m_capacity) {
            m_size -= m_cache.front().get_size();
            m_cache.pop_front();
          }

          m_cache.emplace_back(static_cast<int>(row), static_cast<int>(col), size,
            vector_element, band);
          vector_element = --(m_cache.end());
        }

        std::list<block> m_cache;
        size_t m_size;
        size_t m_capacity;
      };

      //! The global block_cache. This is a block cache with default size that can
      //! be used as a default when none is provided.
      block_cache g_block_cache;

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
        block_manager(GDALRasterBand* band, block_cache* cache = &g_block_cache)
          : m_block_cache(cache), m_gdal_rasterband(band)
        {
          int block_size1, block_size2;
          m_gdal_rasterband->GetBlockSize(&block_size2, &block_size1);
          const size_t element_bit_size = GDALGetDataTypeSize(
            m_gdal_rasterband->GetRasterDataType());
          const size_t element_byte_size = 1 + (element_bit_size - 1) / 8;
          m_block_size = block_size1 * block_size2 * element_byte_size;
          m_major_size1 = 1 + (m_gdal_rasterband->GetYSize() - 1) / block_size1;
          m_major_size2 = 1 + (m_gdal_rasterband->GetXSize() - 1) / block_size2;
          m_index.assign(m_major_size1 * m_major_size2, boost::none);
        }

        block_manager() : m_block_cache(nullptr), m_gdal_rasterband(nullptr)
        {

        }

        block_manager(block_manager&& that)
          : m_index(std::move(that.m_index))
          , m_block_cache(std::move(that.m_block_cache))
          , m_gdal_rasterband(that.m_gdal_rasterband)
          , m_block_size(that.m_block_size)
          , m_major_size1(that.m_major_size1)
          , m_major_size2(that.m_major_size2)
        {
          that.m_gdal_rasterband = nullptr;
        }

        ~block_manager()
        {
          flush();
        }

        block_manager& operator=(block_manager&& that)
        {
          if (this != &that) {
            flush();
            m_index = std::move(that.m_index);
            m_block_cache = std::move(that.m_block_cache);
            m_gdal_rasterband = that.m_gdal_rasterband;
            m_block_size = that.m_block_size;
            m_major_size1 = that.m_major_size1;
            m_major_size2 = that.m_major_size2;
          }
          return *this;
        }

        void flush()
        {
          for (std::vector<optional_block_list_iterator>::iterator i
            = m_index.begin(); i != m_index.end(); ++i) {
            if (*i) {
              m_block_cache->remove(*(*i));
              *i = boost::none;
            }
          }
        }

        //! Get a writable block by its block index
        //!
        void* get_for_writing(size_t index)
        {
          block_list_iterator i = get_list_iterator(index);
          i->mark_dirty();
          return i->get_buffer();
        }

        //! Get a read only block by its block index
        //!
        const void* get(size_t index) const
        {
          block_list_iterator i = get_list_iterator(index);
          return i->get_buffer();
        }
      private:
        block_list_iterator get_list_iterator(size_t index) const
        {
          optional_block_list_iterator& element = m_index[index];
          if (element) {
            // Move to back
            m_block_cache->touch(*element);
          }
          else {
            // Create new
            m_block_cache->make(index / m_major_size2, index % m_major_size2,
              m_block_size, element, m_gdal_rasterband);
          }
          return *element;
        }


        mutable std::vector<optional_block_list_iterator> m_index;
        block_cache* m_block_cache;
        GDALRasterBand* m_gdal_rasterband;
        size_t m_block_size;
        size_t m_major_size1;
        size_t m_major_size2;
      };
    }; //LRU namespace

  }
}
#endif