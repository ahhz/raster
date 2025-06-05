//=======================================================================
// Copyright 2015-2107
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once

#include <pronto/raster/gdal_includes.h>
#include <pronto/raster/iterator_facade.h>
#include <pronto/raster/reference_proxy.h>

#include <cstdint>
#include <memory> //shared_ptr

namespace pronto
{
  namespace raster
  {
    template<class T, iteration_type IterationType = iteration_type::multi_pass, access AccessType = access::read_write>
    class block_iterator
      : public iterator_facade<block_iterator<T, IterationType, AccessType>>
    {

    public:
      static const bool is_single_pass = IterationType == iteration_type::single_pass;
      static const bool is_mutable = AccessType != access::read_only;
      using value_type = T;

      block_iterator()
        : m_pos(nullptr)
      {}

      block_iterator(GDALDataType data_type, GDALAccess access_type, char* block_start)
        : m_pos(block_start)
      {
        switch (data_type)
        {
        case GDT_Byte:     set_accessors<uint8_t >(access_type);   break;
        case GDT_Int16:    set_accessors<int16_t >(access_type);   break;
        case GDT_UInt16:   set_accessors<uint16_t>(access_type);   break;
        case GDT_Int32:    set_accessors<int32_t>(access_type);    break;
        case GDT_UInt32:   set_accessors<uint32_t>(access_type);   break;
        case GDT_Float32:  set_accessors<float>(access_type);      break;
        case GDT_Float64:  set_accessors<double>(access_type);     break;
          // Complex numbers not currently supported
          //
          //case GDT_CInt16:   set_accessors<cint16_t>();   break;
          //case GDT_CInt32:   set_accessors<cint32_t>();   break;
          //case GDT_CFloat32: set_accessors<cfloat32_t>(); break;
          //case GDT_CFloat64: set_accessors<cfloat64_t>(); break;
        default: break;

        }
        m_stride = GDALGetDataTypeSize(data_type) / 8;
      }

      block_iterator(const block_iterator&) = default;
      block_iterator(block_iterator&&) = default;
      block_iterator& operator=(const block_iterator&) = default;
      block_iterator& operator=(block_iterator&&) = default;
      ~block_iterator() = default;

      auto dereference() const {
        if constexpr (is_mutable)
        {
          if constexpr (is_single_pass)
          {
            return put_get_proxy_reference<const block_iterator&>(*this);
          }
          else {
            return put_get_proxy_reference<block_iterator>(*this);
          }
        }
        else {
          return get();
        }
      }

      void increment() {
        m_pos += m_stride;
      }

      void decrement() {
        m_pos -= m_stride;
      }

      void advance(std::ptrdiff_t offset) {
        m_pos += offset * m_stride;
      }

      bool equal_to(const block_iterator& other) const {
        return m_pos == other.m_pos;
      }

      std::ptrdiff_t distance_to(const block_iterator& other) const {
        return (other.m_pos - m_pos) / m_stride;
      }

      template<typename U>
      static void put_special(const value_type& value, void* const target)
      {
        *(static_cast<U*>(target)) = static_cast<U>(value);
      }

      static void put_nothing(const value_type& value, void* const target)
      {
        throw(writing_to_raster_failed{});
      }

      template<typename U>
      static value_type get_special(const void* const source)
      {
        return static_cast<value_type>(*static_cast<const U*>(source));
      }

      template<typename U> 
      void set_accessors(GDALAccess access_type)
      {
        m_get = block_iterator::get_special<U>;
        m_put = access_type != GA_ReadOnly 
          ? block_iterator::put_special<U> 
          : block_iterator::put_nothing;
      }
 
      friend class put_get_proxy_reference<const block_iterator&>;
      friend class put_get_proxy_reference<block_iterator>;
   
      void put(const value_type& v) const
      {
        m_put(v, static_cast<void*>(m_pos));
      }

      value_type get() const
      {
        return m_get(static_cast<void*>(m_pos));
      }
      // function pointers for runtime polymorphism
      void(*m_put)(const value_type&, void* const);
      value_type(*m_get)(const void* const);
      char* m_pos;
      char m_stride;
    };

    template<class T, iteration_type IterationType = iteration_type::multi_pass, access AccessType = access::read_write>
    struct block
    {
      using iterator = block_iterator<T, IterationType, AccessType>;
  
      block() = default;
     
      void reset(GDALRasterBand* band, int major_row, int major_col)
      {
        // Avoid rereading same block
        if (m_block && m_block->GetBand() == band 
          && major_row == this->major_row() && major_col == this->major_col()) 
          return;

        GDALRasterBlock* block = band->GetLockedBlockRef(major_col, major_row);
        if (block == nullptr) {
          throw(reading_from_raster_failed{});
        }
        auto deleter = [](GDALRasterBlock* b) {b->DropLock(); };
        m_block.reset(block, deleter);
      }

      void reset(int major_row, int major_col)
      {
        GDALRasterBand* band = m_block->GetBand();
        reset(band, major_row, major_col);
      }

      void reset()
      {
        m_block.reset();
      }

      int block_rows() const
      {
        return m_block->GetYSize();
      }

      int block_cols() const
      {
        return m_block->GetXSize();
      }

      int major_row() const
      {
        return m_block->GetYOff();
      }

      int major_col() const
      {
        return m_block->GetXOff();
      }

      iterator begin() const
      {
        char* block_start = static_cast<char*>(m_block->GetDataRef());

        auto data_type = m_block->GetBand()->GetRasterDataType();
        auto access_type = m_block->GetBand()->GetAccess();

        return iterator(data_type, access_type, block_start);
      }
 
      void mark_dirty() const //mutable
      {
        m_block->MarkDirty();
      }

    private:
      mutable std::shared_ptr<GDALRasterBlock> m_block;
    };
  }
}
