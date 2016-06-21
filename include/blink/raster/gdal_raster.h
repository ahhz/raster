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
#include <blink/raster/gdal_input_iterator.h>
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

    /*
    // Forward declaration for friendship;
    template<class T> class typed_gdalrasterband_iterator;
    template<typename U> class gdalrasterband_input_view;

    template<bool IsMutable>
    class gdalrasterband_input_iterator : public std::iterator<std::input_iterator_tag, void*>
    {
    public:

      gdalrasterband_input_iterator() : m_block(nullptr), m_end_of_stretch(nullptr), m_pos_in_stretch(nullptr)
      {}

      gdalrasterband_input_iterator(const gdalrasterband_input_iterator& other)
        : m_block(other.m_block)
        , m_end_of_stretch(other.m_end_of_stretch)
        , m_pos_in_stretch(other.m_pos_in_stretch)
        , m_stride(other.m_stride)
      {
        if (m_block) m_block->AddLock();
      }

      gdalrasterband_input_iterator(gdalrasterband_input_iterator&& other)
        : m_block(other.m_block)
        , m_end_of_stretch(other.m_end_of_stretch)
        , m_pos_in_stretch(other.m_pos_in_stretch)
        , m_stride(other.m_stride)
      {
        other.m_block = nullptr;
        other.m_end_of_stretch = nullptr;
        other.m_pos_in_stretch = nullptr;
      }

      gdalrasterband_input_iterator& operator=(const gdalrasterband_input_iterator& other)
      {
        if (m_block) m_block->DropLock();

        m_block = other.m_block;
        m_end_of_stretch = other.m_end_of_stretch;
        m_pos_in_stretch = other.m_pos_in_stretch;
        m_stride = other.m_stride;

        if (m_block) m_block->AddLock();
      }

      gdalrasterband_input_iterator& operator=(gdalrasterband_input_iterator&& other)
      {
        m_block = other.m_block;
        m_end_of_stretch = other.m_end_of_stretch;
        m_pos_in_stretch = other.m_pos_in_stretch;
        m_stride = other.m_stride;
        if (m_block) m_block->AddLock();
  
        other.m_block = nullptr;
        other.m_end_of_stretch = nullptr;
        other.m_pos_in_stretch = nullptr;
      }

      friend bool operator==(const gdalrasterband_input_iterator& a
        , const gdalrasterband_input_iterator& b)
      {
        return a.m_pos_in_stretch == b.m_pos_in_stretch;
      }

      friend bool operator!=(const gdalrasterband_input_iterator& a
        , const gdalrasterband_input_iterator& b)
      {
        return a.m_pos_in_stretch != b.m_pos_in_stretch;
      }
      ~gdalrasterband_input_iterator()
      {
        if (m_block) m_block->DropLock();
      }

      gdalrasterband_input_iterator& operator++()
      {
        m_pos_in_stretch += m_stride;

        if (m_pos_in_stretch == m_end_of_stretch) {

          int major_row = m_block->GetYOff();
          int major_col = m_block->GetXOff();

          GDALRasterBand* band = m_block->GetBand();

          char* start = static_cast<char*>(m_block->GetDataRef());
          int prev_pos_index = std::distance(start, m_pos_in_stretch) / m_stride - 1;

          int minor_row = prev_pos_index / m_block->GetXSize();
          int num_major_cols = 1 + (band->GetXSize() - 1) / m_block->GetXSize();

          if (++major_col == num_major_cols) {
            major_col = 0;
            ++minor_row;
            int end_minor_row = m_block->GetYSize();
            int num_major_rows = 1 + (band->GetYSize() - 1) / m_block->GetYSize();

            if (major_row == num_major_rows - 1) {
              end_minor_row = band->GetYSize() - (major_row * m_block->GetYSize());
            }
            if (minor_row == end_minor_row) {
              minor_row = 0;
              int num_major_rows = 1 + (band->GetYSize() - 1) / m_block->GetYSize();
              if (++major_row == num_major_rows) {
                // iterator at end
                m_block->DropLock();
                m_block = nullptr;
                m_end_of_stretch = nullptr;
                m_pos_in_stretch = nullptr;
                return *this;
              }
            }
          }
          band = m_block->GetBand();
          m_block->DropLock();
          m_block = band->GetLockedBlockRef(major_col, major_row);
          if (IsMutable) m_block->MarkDirty(); // should depend on access type (TODO)		
          start = static_cast<char*>(m_block->GetDataRef());
          m_pos_in_stretch = start + minor_row * m_block->GetXSize() * m_stride;
          int cols_in_stretch = m_block->GetXSize();
          if (major_col == num_major_cols - 1) {
            cols_in_stretch = band->GetXSize() - (major_col * m_block->GetXSize());
          }
          m_end_of_stretch = m_pos_in_stretch + cols_in_stretch * m_stride;
        }
        return *this;
      }

      gdalrasterband_input_iterator operator++(int)
      {
        gdalrasterband_input_iterator temp(*this);
        ++(*this);
        return temp;
      }

      void* operator*() const
      {
        return static_cast<void*>(m_pos_in_stretch);
      }

    private:

      template<class T>
      friend class typed_gdalrasterband_iterator;

      void find_begin(GDALRasterBand* band)
      {
        if (m_block) m_block->DropLock();
        m_block = band->GetLockedBlockRef(0, 0);
        GDALDataType datatype = band->GetRasterDataType();
        m_stride = GDALGetDataTypeSizeBytes(datatype);
        m_pos_in_stretch = static_cast<char*>(m_block->GetDataRef());
        int n = std::min(m_block->GetXSize(), m_block->GetBand()->GetXSize());
        m_end_of_stretch = m_pos_in_stretch + n * m_stride;
      }

      void find_end(GDALRasterBand* band = nullptr)
      {
        if (m_block) m_block->DropLock();
        m_block = nullptr;
        m_pos_in_stretch = nullptr;
        m_end_of_stretch = nullptr;
      }

    private:
      GDALRasterBlock* m_block;
      char* m_end_of_stretch;
      char* m_pos_in_stretch;
      unsigned char m_stride; 
    };


    template<bool IsMutable>
    class sub_gdalrasterband_input_iterator : public std::iterator<std::input_iterator_tag, void*>
    {
    public:

      sub_gdalrasterband_input_iterator() : m_block(nullptr), m_end_of_stretch(nullptr), m_pos_in_stretch(nullptr)
      {}

      sub_gdalrasterband_input_iterator(const sub_gdalrasterband_input_iterator& other)
        : m_block(other.m_block)
        , m_end_of_stretch(other.m_end_of_stretch)
        , m_pos_in_stretch(other.m_pos_in_stretch)
        , m_stride(other.m_stride)
        , m_start_row(other.m_start_row)
        , m_start_col(other.m_start_col)
        , m_rows(other.m_rows)
        , m_cols(other.m_cols)
      {
        if (m_block) m_block->AddLock();
      }

      sub_gdalrasterband_input_iterator(sub_gdalrasterband_input_iterator&& other)
        : m_block(other.m_block)
        , m_end_of_stretch(other.m_end_of_stretch)
        , m_pos_in_stretch(other.m_pos_in_stretch)
        , m_stride(other.m_stride)
        , m_start_row(other.m_start_row)
        , m_start_col(other.m_start_col)
        , m_rows(other.m_rows)
        , m_cols(other.m_cols)
      {
        other.m_block = nullptr;
        other.m_end_of_stretch = nullptr;
        other.m_pos_in_stretch = nullptr;
      }

      sub_gdalrasterband_input_iterator& operator=(const sub_gdalrasterband_input_iterator& other)
      {
        if (m_block) m_block->DropLock();

        m_block = other.m_block;
        m_end_of_stretch = other.m_end_of_stretch;
        m_pos_in_stretch = other.m_pos_in_stretch;
        m_stride = other.m_stride;
        m_start_row = other.m_start_row;
        m_start_col = other.m_start_col;
        m_rows = other.m_rows;
        m_cols = other.m_cols;

        if (m_block) m_block->AddLock();
      }

      sub_gdalrasterband_input_iterator& operator=(sub_gdalrasterband_input_iterator&& other)
      {
        m_block = other.m_block;
        m_end_of_stretch = other.m_end_of_stretch;
        m_pos_in_stretch = other.m_pos_in_stretch;
        m_stride = other.m_stride;

        m_start_row = other.m_start_row;
        m_start_col = other.m_start_col;
        m_rows = other.m_rows;
        m_cols = other.m_cols;

        if (m_block) m_block->AddLock();

        other.m_block = nullptr;
        other.m_end_of_stretch = nullptr;
        other.m_pos_in_stretch = nullptr;
      }

      friend bool operator==(const sub_gdalrasterband_input_iterator& a
        , const sub_gdalrasterband_input_iterator& b)
      {
        return a.m_pos_in_stretch == b.m_pos_in_stretch;
      }

      friend bool operator!=(const sub_gdalrasterband_input_iterator& a
        , const sub_gdalrasterband_input_iterator& b)
      {
        return a.m_pos_in_stretch != b.m_pos_in_stretch;
      }
      ~sub_gdalrasterband_input_iterator()
      {
        if (m_block) m_block->DropLock();
      }

      sub_gdalrasterband_input_iterator& operator++()
      {
        m_pos_in_stretch += m_stride;

        if (m_pos_in_stretch == m_end_of_stretch) {

          int block_rows = m_block->GetYSize();
          int block_cols = m_block->GetXSize();

          int major_row = m_block->GetYOff();
          int major_col = m_block->GetXOff();

          char* start = static_cast<char*>(m_block->GetDataRef());
          int index_in_block = std::distance(start, m_pos_in_stretch)
            / m_stride - 1;

          int minor_row = index_in_block / block_cols;
          int minor_col = index_in_block % block_cols;

          int row = major_row * block_rows + minor_row;
          int col = major_col * block_cols + minor_col;

          ++minor_col;
          if (minor_col == block_cols) {
            minor_col = 0;
          }
          ++major_col;
          ++col;

          if (col == m_start_col + m_cols) {

            col = m_start_col;
            major_col = m_start_col / block_cols;
            minor_col = m_start_col % block_cols;

            ++minor_row;

            if (minor_row == block_rows) {
              minor_row = 0;
            }
            ++row;
            major_row = row / block_rows;
           
            if (row == m_start_row + m_rows) {
              // iterator at end
              m_block->DropLock();
              m_block = nullptr;
              m_end_of_stretch = nullptr;
              m_pos_in_stretch = nullptr;
              return *this;
            }
          }

          GDALRasterBand* band = m_block->GetBand();
          m_block->DropLock();
          m_block = band->GetLockedBlockRef(major_col, major_row);
          if (IsMutable) m_block->MarkDirty(); 

          start = static_cast<char*>(m_block->GetDataRef());
          
          m_pos_in_stretch = start + 
            (minor_row * block_cols + minor_col) * m_stride;
          
          int end_col = (major_col+1) * block_cols;
          if (end_col > m_start_col + m_cols) {
            end_col = m_start_col + m_cols;
          }
          int minor_end_col = 1 + (end_col - 1) % block_cols;
  
          m_end_of_stretch = start +
            (minor_row * block_cols + minor_end_col) * m_stride;
        }
        return *this;
      }

      sub_gdalrasterband_input_iterator operator++(int)
      {
        sub_gdalrasterband_input_iterator temp(*this);
        ++(*this);
        return temp;
      }

      void* operator*() const
      {
        return static_cast<void*>(m_pos_in_stretch);
      }

    private:

      template<class T>
      friend class typed_gdalrasterband_iterator;

      void find_begin(GDALRasterBand* band
        , int start_row = 0, int start_col = 0, int rows = -1, int cols = -1) 
      {
        m_start_row = start_row;
        m_start_col = start_col;
        m_rows = rows == -1 ? band->GetYSize() : rows;
        m_cols = cols == -1 ? band->GetXSize() : cols;;

        int block_rows = 0;
        int block_cols = 0;
        band->GetBlockSize(&block_cols, &block_rows);

        int major_start_row = start_row / block_rows;
        int major_start_col = start_col / block_cols;

        int minor_start_row = start_row % block_rows;
        int minor_start_col = start_col % block_cols;

        if (m_block) m_block->DropLock();
        m_block = band->GetLockedBlockRef(major_start_row, major_start_col);
        GDALDataType datatype = band->GetRasterDataType();
        m_stride = GDALGetDataTypeSizeBytes(datatype);
        
        char* start = static_cast<char*>(m_block->GetDataRef());

        m_pos_in_stretch = start + 
          (minor_start_row * block_cols + minor_start_col) * m_stride;
        
        int end_col = (major_start_col + 1) * block_cols;
        if (end_col > m_start_col + m_cols) {
          end_col = m_start_col + m_cols;
        }

        int minor_end_col = 1 + (end_col - 1) % m_block->GetXSize();

        m_end_of_stretch = start +
          (minor_start_row * block_cols + minor_end_col) * m_stride;

      }

      void find_end(GDALRasterBand* band = nullptr)
      {
        if (m_block) m_block->DropLock();
        m_block = nullptr;
        m_pos_in_stretch = nullptr;
        m_end_of_stretch = nullptr;
      }

    private:
      GDALRasterBlock* m_block;
      char* m_end_of_stretch;
      char* m_pos_in_stretch;
      unsigned char m_stride; // TODO: get from view pointer, to reduce size?
      int m_start_row;// TODO: get from view?
      int m_start_col;// TODO: get from view?
      int m_rows;// TODO: get from view?
      int m_cols;// TODO: get from view?
    };

    template<class T>
    class typed_gdalrasterband_iterator
      : public std::iterator<std::input_iterator_tag, T
      , dereference_proxy<typed_gdalrasterband_iterator<T>, T> >
    {
      using reference = dereference_proxy<typed_gdalrasterband_iterator<T>, T>;
  
    public:
      
      typed_gdalrasterband_iterator() : m_view(nullptr)
      {};

      typed_gdalrasterband_iterator(const typed_gdalrasterband_iterator& other)
        = default;
      typed_gdalrasterband_iterator(typed_gdalrasterband_iterator&& other)
        = default;
      typed_gdalrasterband_iterator& operator=(
        const typed_gdalrasterband_iterator& other) = default;
      typed_gdalrasterband_iterator& operator=(
        typed_gdalrasterband_iterator&& other) = default;
      ~typed_gdalrasterband_iterator() = default;

      typed_gdalrasterband_iterator& operator++()
      {
        ++m_iter;
        return *this;
      }

      typed_gdalrasterband_iterator operator++(int)
      {
        typed_gdalrasterband_iterator temp(*this);
        ++(*this);
        return temp;
      }

      reference operator*() const
      {
        return reference(this);
      }

      T get() const
      {
        return m_view->get(*m_iter);
      }

      void put(const T& value) const
      {
        m_view->put(value, *m_iter);
      }
   
    public:
      friend bool operator==(const typed_gdalrasterband_iterator& a,
        const typed_gdalrasterband_iterator& b)
      {
        return a.m_iter == b.m_iter;
      }

      friend bool operator!=(const typed_gdalrasterband_iterator& a,
        const typed_gdalrasterband_iterator& b)
      {
        return a.m_iter != b.m_iter;
      }     

    private:

      template<typename U>
      friend class gdalrasterband_input_view;

      void find_begin(gdalrasterband_input_view<T>* view)
      {
        m_view = view;
        m_iter.find_begin(m_view->m_band);
      }

      void find_end(gdalrasterband_input_view<T>* view)
      {
        m_view = view;
        m_iter.find_end(m_view->m_band);
      }

    private:
      const static bool IsMutable = !std::is_const<T>::value;
      sub_gdalrasterband_input_iterator<IsMutable> m_iter;
      gdalrasterband_input_view<T>* m_view;
    };

    // if you want this view to be read_only specify a const T
    template<typename T>
    class gdalrasterband_input_view
    {
      using this_type = gdalrasterband_input_view<T>;
   
      
    public:
      gdalrasterband_input_view() :m_band(nullptr)
      {}

      gdalrasterband_input_view(GDALRasterBand* band) :m_band(band)
      {
        switch (m_band->GetRasterDataType())
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
        if (std::is_const<T>::value) {
          put = this_type::put_nothing;
        }
      }
      template<typename U> void set_accessors()
      {
        put = this_type::put_special<U>;
        get = this_type::get_special<U>;
      }


      using value_type = T;
      using iterator = typed_gdalrasterband_iterator<T>;

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
      
      GDALRasterBand* m_band;

      // function pointers for "runtime polymorphism" based on file datatype. 
      void(*put)(const value_type&, void* const);
      value_type(*get)(const void* const);

      template<typename U>
      static void put_special(const value_type& value, void* const target)
      {
        U* target_cast = static_cast<U*>(target);
        *target_cast = static_cast<U>(value);
      }

      static void put_nothing(const value_type& value, void* const target)
      {
        assert(false);
      }

      template<typename U> 
      static value_type get_special(const void* const source)
      {
        const U* source_cast = static_cast<const U*>(source);
        return static_cast<value_type>(*source_cast);
      }
    };
    */
  } //namespace 
}
#endif