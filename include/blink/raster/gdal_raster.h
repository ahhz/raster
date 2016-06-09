//
//=======================================================================
// Copyright 2015
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// This header file provides a wrapper around a GDALDataSet that allows it 
// to be used by the gdal_raster_iterator (which iterates row-by-row or 
// column-by-column) over the rasterdata. 
//
// This class hides much of the complexity of GDALDataSet and makes it 
// efficiently accessible through iterators. It optionally uses its own disk
// caching and not gdal's. 
//

#ifndef BLINK_RASTER_GDAL_RASTER_H_AHZ
#define BLINK_RASTER_GDAL_RASTER_H_AHZ

//#include <moving_window/coordinate_2d.h>
//#include <moving_window/exceptions.h>
#include <blink/raster/coordinate_2d.h>
#include <blink/raster/gdal_raster_iterator.h>
#include <blink/raster/raster_iterator.h>
#include <blink/raster/raster_traits.h>

#include <boost/filesystem.hpp>

#pragma warning( push )
#pragma warning( disable : 4251 )
#include <cpl_string.h> // part of GDAL
#include <gdal.h>       // part of GDAL
#include <gdal_priv.h>  // part of GDAL
#pragma warning( pop ) 

//#include <utility>
#include <cassert>

namespace blink {
  namespace raster {
  
    template<typename T>
    class gdal_raster
    {
    private:
      using this_type =  gdal_raster<T>;

    public:
      using coordinate_type = coordinate_2d;
      using index_type = coordinate_2d::index_type; //std::ptrdiff_t
      using value_type = T;
      
      using iterator = gdal_iterator<this_type>;
      using const_iterator = gdal_iterator<const this_type>;
  
      // Default constructor
      //
      gdal_raster()
        : m_band(0)
        , m_block_size1(1)
        , m_block_size2(1)
        , m_delete_on_close(false)
        , m_gdal_dataset(nullptr)
        , m_gdal_rasterband(nullptr)
      {}

    public:
      // Consider making this private
      // friend class detail::gdal_makers;
      //
      // Construct from opened GDALDataset
      //
      gdal_raster(GDALDataset* dataset, int band = 1)
        : m_delete_on_close(false)
        , m_gdal_dataset(dataset)
      {
        m_gdal_rasterband = m_gdal_dataset->GetRasterBand(band);
        int x_size, y_size;

        m_gdal_rasterband->GetBlockSize(&x_size, &y_size);

        m_block_size1 = static_cast<index_type>(y_size);
        m_block_size2 = static_cast<index_type>(x_size);
   
        // Using pointers to member functions as a means of run-time polymorphism
        switch (m_gdal_rasterband->GetRasterDataType())
        {
        case GDT_Byte:    set_accessors<unsigned char >(); break;
        case GDT_Int16:   set_accessors<short         >(); break;
        case GDT_UInt16:  set_accessors<unsigned short>(); break;
        case GDT_Int32:   set_accessors<int           >(); break;
        case GDT_UInt32:  set_accessors<unsigned int  >(); break;
        case GDT_Float32: set_accessors<float         >(); break;
        case GDT_Float64: set_accessors<double        >(); break;
        default: break;
        }
        if (m_gdal_dataset->GetAccess() == GA_ReadOnly) {
          put_pixel = this_type::put_nothing;
        }
      }

      void set_delete_on_close(bool v)
      {
        m_delete_on_close = v;
      }

      bool get_delete_on_close() const
      {
        return m_delete_on_close;
      }

    public:
      // Move constructor only
      //
      gdal_raster(const gdal_raster&) = delete;

      gdal_raster(gdal_raster&& other)
      {
        m_block_size1 = other.m_block_size1;
        m_block_size2 = other.m_block_size2;
        m_delete_on_close = other.m_delete_on_close;
     
        m_gdal_dataset = other.m_gdal_dataset;
        m_gdal_rasterband = other.m_gdal_rasterband;
        
        put_pixel = other.put_pixel;
        get_pixel = other.get_pixel;

        // Make sure other can be safely deleted
        other.m_delete_on_close = false;
        other.m_gdal_dataset = nullptr;
        other.m_gdal_rasterband = nullptr;
      }

      // Move assignment only 
      //
      gdal_raster& operator=(const gdal_raster&) = delete;

      gdal_raster& operator=(gdal_raster&& other)
      {
        close_dataset();

        m_block_size1 = other.m_block_size1;
        m_block_size2 = other.m_block_size2;
        m_delete_on_close = other.m_delete_on_close;
        m_gdal_dataset = other.m_gdal_dataset;
        m_gdal_rasterband = other.m_gdal_rasterband;
        put_pixel = other.put_pixel;
        get_pixel = other.get_pixel;

        // Make sure other can be safely deleted
        other.m_delete_on_close = false;
        other.m_gdal_dataset = nullptr;
        other.m_gdal_rasterband = nullptr;
        return *this;
      }

      ~gdal_raster()
      {
        close_dataset();
      }
            
      const_iterator begin() const
      {
        const_iterator i(this);
        i.find_begin();
        return i;
      }

      const_iterator end() const
      {
        const_iterator i(this);
        i.find_end();
        return i;
      }

      iterator begin()
      {
        iterator i(this);
        i.find_begin();
        return i;
      }

      iterator end()
      {
        iterator i(this);
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
        return m_gdal_dataset->GetRasterYSize();
      }

      index_type size2() const
      {
        return m_gdal_dataset->GetRasterXSize();
      }

      // Pixel access
      //
      T get(const coordinate_type& coord) const
      {
        return get_pixel_in_block(
          coord.row / m_block_size1,
          coord.col / m_block_size2,
          (coord.row % m_block_size1) * m_block_size2 
          + coord.col % m_block_size2);
        }
      
      void put(const coordinate_type& coord, const T& value)
      {
        get_pixel_in_block(
          coord.row / m_block_size1,
          coord.col / m_block_size2,
          (coord.row % m_block_size1) * m_block_size2
          + coord.col % m_block_size2,
          value);
      }

      index_type block_size1() const
      {
        return m_block_size1;
      }

      index_type block_size2() const
      {
        return m_block_size2;
      }

      T get_pixel_in_block(index_type block, index_type index_in_block) const
      {
        return get_pixel_in_block(block / m_block_size1, block % m_block_size1,
          index_in_block);
      }

      T get_pixel_in_block(index_type blockrow, index_type blockcol
        , index_type index_in_block) const
      {
        auto block = m_gdal_rasterband->GetLockedBlockRef(
          static_cast<int>(blockcol), static_cast<int>(blockrow));
        const void* buf = block->GetDataRef();
        T temp = get_pixel(buf, index_in_block);
        block->Touch();
        block->DropLock();
        return temp;
      }

      void put_pixel_in_block(index_type block, index_type index_in_block,
        const T& value)
      {
        put_pixel_in_block(block / m_block_size1, block % m_block_size1,
          index_in_block, value);
      }

      void put_pixel_in_block(index_type blockrow, index_type blockcol, 
        index_type index_in_block, const T& value)
      {
        auto block = m_gdal_rasterband->GetLockedBlockRef(
          static_cast<int>(blockcol), static_cast<int>(blockrow));
        void* const buf = block->GetDataRef();
        put_pixel(value, buf, index_in_block);
        block->MarkDirty();
        block->Touch();
        block->DropLock();
      }

      const GDALDataset* get_gdal_dataset() const
      {
        return m_gdal_dataset;
      }

    private:
      void close_dataset()
      {
        if (m_gdal_dataset && m_gdal_dataset->GetAccess() == GA_Update) {
          double min, max, mean, stddev;
          m_gdal_rasterband->ComputeStatistics(FALSE, &min, &max, &mean, &stddev
            , NULL, NULL);
          m_gdal_rasterband->SetStatistics(min, max, mean, stddev);
        }

        if (m_gdal_dataset) {
          char** file_list = m_gdal_dataset->GetFileList();
          GDALClose(m_gdal_dataset);
          if (m_delete_on_close) {
            if (file_list != nullptr) {
              for (int i = 0; file_list[i] != nullptr; ++i) {
                boost::filesystem::path path(file_list[i]);
                boost::filesystem::remove(path);
              }
            }
          }
          CSLDestroy(file_list);
        }
      }
   
      template<typename TData, index_type stride = sizeof(TData)>
      static void put_pixel_specialized(const value_type& value, void* const buf
        , index_type n)
      {
        char* const point_at_char = reinterpret_cast<char* const>(buf)+n*stride;
        TData* const point_at_data = reinterpret_cast<TData* const>(point_at_char);
        (*point_at_data) = static_cast<TData>(value);
      }

      template<typename TData, index_type stride = sizeof(TData)>
      static value_type get_pixel_specialized(const void* const buf, index_type n)
      {
        const char* const point_at_char = reinterpret_cast<const char* const>(buf)
          +n * stride;
        const TData* const point_at_data = reinterpret_cast<const TData* const>(
          point_at_char);
        return static_cast<value_type>(point_at_data[0]);
      }

      // to do nothing when putting values in a read only raster
      static void put_nothing(const value_type& value, void* const buf
        , index_type n)
      {
        assert(false); //trying to put values in a write only raster
      }

      template<typename TData>
      void set_accessors()
      {
        put_pixel = this_type::put_pixel_specialized<TData>;
        get_pixel = this_type::get_pixel_specialized<TData>;
      }

      // function pointers for "runtime polymorphism" based on file datatype. 
      void(*put_pixel)(const value_type&, void* const, index_type);
      value_type(*get_pixel)(const void* const, index_type);

      index_type m_block_size1;
      index_type m_block_size2;
      bool m_delete_on_close;
     
      // mutable because not const - correctly implemented
      mutable GDALDataset* m_gdal_dataset;
      mutable GDALRasterBand* m_gdal_rasterband;
    };
  } //namespace 
}
#endif