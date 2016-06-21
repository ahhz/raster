//
//=======================================================================
// Copyright 2016
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// This header file provides a view around a GDALDataSet that allows it 
// to be used as a (mutable) input_range that iterates row-by-row 
//
// This class hides much of the complexity of GDALDataSet and makes it 
// efficiently accessible through iterators. 
//
// This is work in progres to replace the gdal_raster_view and gdal_iterator. 
// The problem with those is that they do not conform strictly to the input 
// iterator and output iterator concepts

#ifndef BLINK_RASTER_GDAL_INPUT_ITERATOR_H_AHZ
#define BLINK_RASTER_GDAL_INPUT_ITERATOR_H_AHZ

#pragma warning( push )
#pragma warning( disable : 4251 )
#include <cpl_string.h> // part of GDAL
#include <gdal.h>       // part of GDAL
#include <gdal_priv.h>  // part of GDAL
#pragma warning( pop ) 

//#include <utility>
#include <cassert>
#include <memory>

namespace blink {
  namespace raster {
    //// Forward declaration for friendship;
    template<class T> class typed_gdalrasterband_iterator;
    template<typename U> class gdalrasterband_input_view;
/*
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
    */
    template<bool IsMutable>
    struct block
    {
      using iterator = char*;

      block() : m_block(nullptr)
      {}

      block(GDALRasterBand* band, int major_row, int major_col)
      {
        m_block = band->GetLockedBlockRef(major_col, major_row);
        if (IsMutable) m_block->MarkDirty();
      }

      block(const block& other)
      {
        m_block = other.m_block;
        if(m_block) m_block->AddLock();
      }

      block(block&& other)
      {
        m_block = other.m_block;
        other.m_block = nullptr;
      }

      block& operator=(const block& other)
      {
        if (m_block) m_block->DropLock();
        m_block = other.m_block;
        if (m_block) m_block->AddLock();
        return *this;
      }

      block& operator=(block&& other)
      {
        if (m_block) m_block->DropLock();
        m_block = other.m_block;
        other.m_block = nullptr;
        return *this;
      }

      void reset(GDALRasterBand* band, int major_row, int major_col)
      {
        if(m_block) m_block->DropLock();
        m_block = band->GetLockedBlockRef(major_col, major_row);
        if (IsMutable) m_block->MarkDirty();
      }

      void reset(int major_row, int major_col)
      {
        GDALRasterBand* band = m_block->GetBand();
        if(m_block) m_block->DropLock();
        m_block = band->GetLockedBlockRef(major_col, major_row);
        if (IsMutable) m_block->MarkDirty();
      }

      void reset()
      {
        if (m_block) m_block->DropLock();
        m_block = nullptr;
      }

      ~block()
      {
        if(m_block) m_block->DropLock();
      }

      int block_rows()
      {
        return m_block->GetYSize();
      }

      int block_cols()
      {
        return m_block->GetXSize();
      }

      int major_row()
      {
        return m_block->GetYOff();
      }

      int major_col()
      {
        return m_block->GetXOff();
      }

      iterator get_iterator(int minor_row, int minor_col, unsigned char stride)
      {
        char* block_start = static_cast<char*>(m_block->GetDataRef());
        int minor_cols = m_block->GetXSize();
        return block_start + stride * (minor_row * minor_cols + minor_col);
      }

      iterator get_null_iterator()
      {
        return nullptr;
      }

    private:
      GDALRasterBlock* m_block;
    };

    template<bool IsMutable>
    class sub_gdalrasterband_input_iterator : public std::iterator<std::input_iterator_tag, void*>
    {
      using block_type = block<IsMutable>;
      using block_iterator_type = typename block_type::iterator;
    public:

      sub_gdalrasterband_input_iterator() 
        : m_block()
        , m_end_of_stretch(m_block.get_null_iterator()) // not so elegant
        , m_pos(m_block.get_null_iterator()) // not so elegant
      {}

      sub_gdalrasterband_input_iterator
        (const sub_gdalrasterband_input_iterator& other) = default;
      
      sub_gdalrasterband_input_iterator
        (sub_gdalrasterband_input_iterator&& other) = default;
  
      sub_gdalrasterband_input_iterator& operator=
        (const sub_gdalrasterband_input_iterator& other) = default;

      sub_gdalrasterband_input_iterator& operator=
        (sub_gdalrasterband_input_iterator&& other) = default;

      ~sub_gdalrasterband_input_iterator() = default;

      friend bool operator==(const sub_gdalrasterband_input_iterator& a
        , const sub_gdalrasterband_input_iterator& b)
      {
        return a.m_pos == b.m_pos;
      }

      friend bool operator!=(const sub_gdalrasterband_input_iterator& a
        , const sub_gdalrasterband_input_iterator& b)
      {
        return a.m_pos != b.m_pos;
      }

      sub_gdalrasterband_input_iterator& operator++()
      {
        m_pos += m_stride;

        if (m_pos == m_end_of_stretch) {

          int block_rows = m_block.block_rows();
          int block_cols = m_block.block_cols();
          
          int major_row = m_block.major_row();
          int major_col = m_block.major_col();

          block_iterator_type start = m_block.get_iterator(0, 0, m_stride);
          
          int index_in_block = std::distance(start, m_pos)
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
              m_block.reset();
              m_end_of_stretch = m_block.get_null_iterator();
              m_pos = m_block.get_null_iterator();
              return *this;
            }
          }

          m_block.reset(major_row, major_col);
          m_pos = m_block.get_iterator(minor_row, minor_col, m_stride);
          
          int end_col = std::min( (major_col + 1) * block_cols, 
            m_start_col + m_cols);
          
          int minor_end_col = 1 + (end_col - 1) % block_cols;

          m_end_of_stretch = m_block.get_iterator(minor_row, minor_end_col, m_stride);
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
        return static_cast<void*>(m_pos);
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

        m_block.reset(band, major_start_row, major_start_col);

        GDALDataType datatype = band->GetRasterDataType();
        m_stride = GDALGetDataTypeSizeBytes(datatype);

        m_pos = m_block.get_iterator(minor_start_row,
          minor_start_col, m_stride);
        
        int end_col = std::min((major_start_col + 1) * block_cols,
          m_start_col + m_cols);

        int minor_end_col = 1 + (end_col - 1) % block_cols;

        m_end_of_stretch = m_block.get_iterator(minor_start_row ,
          minor_end_col, m_stride);
      }

      void find_end(GDALRasterBand* band = nullptr)
      {
        m_block.reset();
        m_pos = m_block.get_null_iterator();
        m_end_of_stretch = m_block.get_null_iterator();
      }

    private:
      block_type m_block;
      block_iterator_type m_end_of_stretch;
      block_iterator_type m_pos;
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
      using const_iterator = typed_gdalrasterband_iterator<const T>;

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

      const_iterator begin() const
      {
        const_iterator i;
        i.find_begin(this);
        return i;
      }

      const_iterator end() const
      {
        const_iterator i;
        i.find_end(this);
        return i;
      }

      const_iterator cbegin() const
      {
        const_iterator i;
        i.find_begin(this);
        return i;
      }

      const_iterator cend() const
      {
        const_iterator i;
        i.find_end(this);
        return i;
      }

    private:
   
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

      public:
      GDALRasterBand* m_band;

      // function pointers for "runtime polymorphism" based on file datatype. 
      void(*put)(const value_type&, void* const);
      value_type(*get)(const void* const);

    };
  }
}
#endif

