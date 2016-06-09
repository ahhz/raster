#include <blink/raster/gdal_raster_lru.h>

#pragma warning( push )
#pragma warning( disable : 4251 )//std::vector needs to have dll-interface 
#include <cpl_conv.h>
#include <gdal_priv.h>
#pragma warning( pop )


// This is the global block cache that is used as a default

namespace blink {
  namespace raster {
    namespace LRU {
      block_cache g_block_cache;
    }
  }
}

blink::raster::LRU::block::block(int row, int col, std::size_t size,
  optional_block_list_iterator& vector_element, GDALRasterBand* band)
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

blink::raster::LRU::block::~block()
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

void blink::raster::LRU::block::mark_dirty()
{
  m_is_dirty = true;
}

size_t blink::raster::LRU::block::get_size() const
{
  return m_buffer.size();
}

void* blink::raster::LRU::block::get_buffer()
{
  return static_cast<void*>(&m_buffer[0]);
}

const void* blink::raster::LRU::block::get_buffer() const
{
  return static_cast<const void*>(&m_buffer[0]);
}

blink::raster::LRU::block_cache::block_cache(std::size_t capacity) 
  : m_capacity(capacity), m_size(0)
{}

blink::raster::LRU::block_cache::block_cache(
  blink::raster::LRU::block_cache&& that)
  : m_cache(std::move(that.m_cache))
  , m_size(that.m_size)
  , m_capacity(that.m_capacity)
{}


blink::raster::LRU::block_cache& 
  blink::raster::LRU::block_cache::operator=(block_cache&& that)
{
  if (this != &that) {
    m_cache = std::move(that.m_cache);
    m_size = that.m_size;
    m_capacity = that.m_capacity;
  }
  return *this;
}

void blink::raster::LRU::block_cache::touch(block_list_iterator& iter)
{
  m_cache.splice(m_cache.end(), m_cache, iter);
}

void blink::raster::LRU::block_cache::remove(block_list_iterator& iter)
{
  m_size -= iter->get_size();
  m_cache.erase(iter); // iter is deleted by now
}

void blink::raster::LRU::block_cache::make(
  size_t row,
  size_t col,
  size_t size,
  optional_block_list_iterator& vector_element, // may be boost::none 
  GDALRasterBand* band)
{
  if (size > m_capacity) {
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


  //! The constructor
  //! \param band the pointer to the band that will be accessed through this 
  //! iterator
  //! \param cache the cache in which block data will be held.By default this
  //! is the global cache
blink::raster::LRU::block_manager::block_manager(GDALRasterBand* band, 
  block_cache* cache)
  : m_block_cache(cache), m_gdal_rasterband(band)
{
  if (!cache) { // if none specified use the global cache
    m_block_cache = &blink::raster::LRU::g_block_cache;
  }

  int block_size1, block_size2;
  m_gdal_rasterband->GetBlockSize(&block_size2, &block_size1);
  const std::size_t element_bit_size = GDALGetDataTypeSize(
    m_gdal_rasterband->GetRasterDataType());
  const std::size_t element_byte_size = 1 + (element_bit_size - 1) / 8;
  m_block_size = block_size1 * block_size2 * element_byte_size;
  m_major_size1 = 1 + (m_gdal_rasterband->GetYSize() - 1) / block_size1;
  m_major_size2 = 1 + (m_gdal_rasterband->GetXSize() - 1) / block_size2;
  m_index.assign(m_major_size1 * m_major_size2, boost::none);
}

blink::raster::LRU::block_manager::block_manager() : m_block_cache(nullptr), m_gdal_rasterband(nullptr)
{

}

blink::raster::LRU::block_manager::block_manager(block_manager&& that)
  : m_index(std::move(that.m_index))
  , m_block_cache(std::move(that.m_block_cache))
  , m_gdal_rasterband(that.m_gdal_rasterband)
  , m_block_size(that.m_block_size)
  , m_major_size1(that.m_major_size1)
  , m_major_size2(that.m_major_size2)
{
  that.m_gdal_rasterband = nullptr;
}

blink::raster::LRU::block_manager::~block_manager()
{
  flush();
}

blink::raster::LRU::block_manager& 
  blink::raster::LRU::block_manager::operator=(
  blink::raster::LRU::block_manager&& that)
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

void blink::raster::LRU::block_manager::flush()
{
  for (auto i : m_index) {
    if (i) {
      m_block_cache->remove(*(i));
      i = boost::none;
    }
  }
}

  //! Get a writable block by its block index
  //!
void* blink::raster::LRU::block_manager::get_for_writing(size_t index)
{
  block_list_iterator i = get_list_iterator(index);
  i->mark_dirty();
  return i->get_buffer();
}

//! Get a read only block by its block index
//!
const void* blink::raster::LRU::block_manager::get(std::size_t index) const
{
  block_list_iterator i = get_list_iterator(index);
  return i->get_buffer();
}

blink::raster::LRU::block_manager::block_list_iterator 
  blink::raster::LRU::block_manager::get_list_iterator(std::size_t index) const
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

