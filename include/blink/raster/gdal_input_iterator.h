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

#ifndef BLINK_RASTER_GDAL_INPUT_ITERATOR_H_AHZ
#define BLINK_RASTER_GDAL_INPUT_ITERATOR_H_AHZ

#include <blink/raster/dereference_proxy.h>

#pragma warning( push )
#pragma warning( disable : 4251 )

#include <gdal.h>       // part of GDAL: GDALDataType enum
#include <gdal_priv.h>  // part of GDAL: GDALRasterBand / GDALRasterBlock

#pragma warning( pop ) 

//#include <utility>
#include <cassert>
#include <memory>

namespace blink {
  namespace raster {
    //// Forward declaration for friendship;


    //template<class T> class typed_gdalrasterband_iterator;
    //template<typename U> class gdalrasterband_input_view;

    template<bool IsMutable>
    struct block
    {
      using iterator = char*;

      block() : m_block(nullptr)
      {}

      ~block()
      {
        if (m_block) m_block->DropLock();
      }

      block(GDALRasterBand* band, int major_row, int major_col)
      {
        m_block = band->GetLockedBlockRef(major_col, major_row);
        if (IsMutable) m_block->MarkDirty();
      }

      block(const block& other)
      {
        m_block = other.m_block;
        if (m_block) m_block->AddLock();
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
        if (m_block) m_block->DropLock();
        m_block = band->GetLockedBlockRef(major_col, major_row);
        if (IsMutable) m_block->MarkDirty();
      }

      void reset(int major_row, int major_col)
      {
        GDALRasterBand* band = m_block->GetBand();
        if (m_block) m_block->DropLock();
        m_block = band->GetLockedBlockRef(major_col, major_row);
        if (IsMutable) m_block->MarkDirty();
      }

      void reset()
      {
        if (m_block) m_block->DropLock();
        m_block = nullptr;
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

    //Forward declaration
    template<class T>
    class gdalrasterband_range_view;

    template<bool IsMutable, class T>
    class gdalrasterband_iterator
      : public std::iterator<std::input_iterator_tag, T
      , dereference_proxy<gdalrasterband_iterator<IsMutable, T>, T> >
    {
      using block_type = block<IsMutable>;
      using block_iterator_type = typename block_type::iterator;
      using this_type = gdalrasterband_iterator<IsMutable, T>;

    public:
      using reference = dereference_proxy<this_type, T>;

      gdalrasterband_iterator()
        : m_block()
        , m_end_of_stretch(m_block.get_null_iterator()) // not so elegant
        , m_pos(m_block.get_null_iterator()) // not so elegant
      {}

      gdalrasterband_iterator
      (const gdalrasterband_iterator& other) = default;

      gdalrasterband_iterator(gdalrasterband_iterator&& other)
        : m_view(other.m_view)
        , m_block(std::move(other.m_block))
        , m_end_of_stretch(std::move(other.m_end_of_stretch))
        , m_pos(std::move(other.m_pos))
      {
      }

      gdalrasterband_iterator& operator=
        (const gdalrasterband_iterator& other) = default;

      gdalrasterband_iterator& operator=
        (gdalrasterband_iterator&& other)
      {
        m_view = other.m_view;
        m_block = std::move(other.m_block);
        m_end_of_stretch = std::move(other.m_end_of_stretch);
        m_pos = std::move(other.m_pos);
        return *this;
      }

      ~gdalrasterband_iterator() = default;

      friend bool operator==(const gdalrasterband_iterator& a
        , const gdalrasterband_iterator& b)
      {
        return a.m_pos == b.m_pos;
      }

      friend bool operator!=(const gdalrasterband_iterator& a
        , const gdalrasterband_iterator& b)
      {
        return a.m_pos != b.m_pos;
      }

      gdalrasterband_iterator& operator++()
      {
        assert(m_pos != m_block.get_null_iterator());
        m_pos += m_view->m_stride;

        if (m_pos == m_end_of_stretch) {

          int block_rows = m_block.block_rows();
          int block_cols = m_block.block_cols();

          int major_row = m_block.major_row();
          int major_col = m_block.major_col();

          block_iterator_type start = m_block.get_iterator(0, 0, m_view->m_stride);

          int index_in_block = static_cast<int>(std::distance(start, m_pos))
            / m_view->m_stride - 1;
          assert(index_in_block >= 0);

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

          if (col == m_view->m_start_col + m_view->m_cols) {

            col = m_view->m_start_col;
            major_col = m_view->m_start_col / block_cols;
            minor_col = m_view->m_start_col % block_cols;

            ++minor_row;

            if (minor_row == block_rows) {
              minor_row = 0;
            }
            ++row;
            major_row = row / block_rows;

            if (row == m_view->m_start_row + m_view->m_rows) {
              // iterator at end
              m_block.reset();
              m_end_of_stretch = m_block.get_null_iterator();
              m_pos = m_block.get_null_iterator();
              return *this;
            }
          }

          m_block.reset(major_row, major_col);
          m_pos = m_block.get_iterator(minor_row, minor_col, m_view->m_stride);

          int end_col = std::min((major_col + 1) * block_cols,
            m_view->m_start_col + m_view->m_cols);

          int minor_end_col = 1 + (end_col - 1) % block_cols;

          m_end_of_stretch = m_block.get_iterator(minor_row, minor_end_col, m_view->m_stride);
        }
        return *this;
      }

      gdalrasterband_iterator operator++(int)
      {
        gdalrasterband_iterator temp(*this);
        ++(*this);
        return temp;
      }

      reference operator*() const
      {
        return reference(this);
      }

      T get() const
      {
        return m_view->get(static_cast<void*>(m_pos));
      }

      void put(const T& value) const
      {
        m_view->put(value, static_cast<void*>(m_pos));
      }

    private:

      friend class gdalrasterband_range_view<T>;
      friend class gdalrasterband_range_view<const T>;

      void find_begin(gdalrasterband_range_view<T>* view)
      {
        m_view = view;

        int block_rows = 0;
        int block_cols = 0;
        m_view->m_band->GetBlockSize(&block_cols, &block_rows);

        int major_start_row = m_view->m_start_row / block_rows;
        int major_start_col = m_view->m_start_col / block_cols;

        int minor_start_row = m_view->m_start_row % block_rows;
        int minor_start_col = m_view->m_start_col % block_cols;

        m_block.reset(m_view->m_band.get(), major_start_row, major_start_col);

        m_pos = m_block.get_iterator(minor_start_row,
          minor_start_col, m_view->m_stride);

        int end_col = std::min((major_start_col + 1) * block_cols,
          m_view->m_start_col + m_view->m_cols);

        int minor_end_col = 1 + (end_col - 1) % block_cols;

        m_end_of_stretch = m_block.get_iterator(minor_start_row,
          minor_end_col, m_view->m_stride);
      }

      void find_end(gdalrasterband_range_view<T>* view)
      {
        m_view = view;
        m_block.reset();
        m_pos = m_block.get_null_iterator();
        m_end_of_stretch = m_block.get_null_iterator();
      }

    private:
      gdalrasterband_range_view<T>* m_view;
      block_type m_block;
      block_iterator_type m_end_of_stretch;
      block_iterator_type m_pos;
    };

    // if you want this view to be read_only specify a const T
    template<typename T>
    class gdalrasterband_range_view
    {
      using this_type = gdalrasterband_range_view<T>;

    public:
      gdalrasterband_range_view() :m_band(nullptr)
      {}

      gdalrasterband_range_view(
        std::shared_ptr<GDALRasterBand> band,
        int start_row = 0, int start_col = 0, int rows = -1, int cols = -1)
        : m_band(band)
      {
        m_start_row = start_row;
        m_start_col = start_col;
        m_rows = rows == -1 ? m_band->GetYSize() : rows;
        m_cols = cols == -1 ? m_band->GetXSize() : cols;
        GDALDataType datatype = band->GetRasterDataType();
        m_stride = GDALGetDataTypeSize(datatype) / 8;


        switch (datatype)
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
      using iterator = gdalrasterband_iterator<!std::is_const<T>::value, T>;
      using const_iterator = gdalrasterband_iterator<false, T>;

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

      int rows() const
      {
        return m_rows;
      }
      int cols() const
      {
        return m_cols;
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
      std::shared_ptr<GDALRasterBand> m_band;

      // function pointers for "runtime polymorphism" based on file datatype. 
      void(*put)(const value_type&, void* const);
      value_type(*get)(const void* const);

      unsigned char m_stride; // todo: make static constant
      int m_start_row;
      int m_start_col;
      int m_rows;
      int m_cols;
    };


    // if you want this view to be read_only specify a const T
    template<class T>
    gdalrasterband_range_view<T> sub_raster(std::shared_ptr<GDALRasterBand> band,
      int start_row, int start_col, int rows, int cols)
    {
      // todo: check whether sub_raster falls within raster
      return gdalrasterband_range_view<T>(band, start_row, start_col, rows,
        cols);
    }

    // if you want this view to be read_only specify a const T
    template<class T>
    gdalrasterband_range_view<T> sub_raster(const gdalrasterband_range_view<T>&
      raster, int start_row, int start_col, int rows, int cols)
    {
      // todo: check whether sub_raster falls within raster
      return gdalrasterband_range_view<T>(raster.m_band,
        raster.m_start_row + start_row,
        raster.m_start_col + start_col, rows, cols);
    }
  }
}
#endif

