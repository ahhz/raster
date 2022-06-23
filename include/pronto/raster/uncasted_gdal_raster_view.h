//=======================================================================
// Copyright 2022
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// This is a variant of gdal_raster_view for the instance where it is assured
// that the GDALDataType is corresponding to the value_type
//

#pragma once

#include <pronto/raster/access_type.h>
#include <pronto/raster/complex_numbers.h>
#include <pronto/raster/exceptions.h>
#include <pronto/raster/gdal_includes.h>
#include <pronto/raster/gdal_raster_iterator.h>
#include <pronto/raster/gdal_raster_view.h>
#include <pronto/raster/nodata_transform.h>
#include <pronto/raster/optional.h>


#include <cassert>
#include <cstdint>
#include <memory>
#include <ranges>
#include <type_traits>
#include <filesystem>
#include <variant>
namespace pronto
{
  namespace raster
  {
    template<class T, access AccessType> // template argument for AccessType is now ignored
    struct uncasted_block
    {
      using value_type = T;
      using iterator = value_type*;

      uncasted_block() = default;

      void reset(GDALRasterBand* band, int major_row, int major_col)
      {
        // Avoid rereading same block
        if (m_block && m_block->GetBand() == band
          && major_row == this->major_row() && major_col == this->major_col())
          return;

        GDALRasterBlock* block = band->GetLockedBlockRef(major_col, major_row);
        if (block == nullptr) {
          assert(false);
          throw("trying to open inaccessible GDALRasterBlock");
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

      iterator get_iterator(int minor_row, int minor_col) const
      {
        auto block_start = static_cast<iterator>(m_block->GetDataRef());
        return block_start + (minor_row * block_cols() + minor_col);
      }

      iterator get_null_iterator() const
      {
        return nullptr;
      }

      void mark_dirty() const //mutable
      {
        m_block->MarkDirty();
      }

    private:
      mutable std::shared_ptr<GDALRasterBlock> m_block;
    };

    template<class, iteration_type, access> class uncasted_gdal_raster_view; // forward declaration

    template<class T, iteration_type IterationType = iteration_type::multi_pass, access AccessType = access::read_write>
    class uncasted_gdal_raster_iterator
      : public iterator_facade<uncasted_gdal_raster_iterator<T, IterationType, AccessType>>
    {
      using block_type = uncasted_block<T, AccessType>;
      using block_iterator_type = typename block_type::iterator;
      using view_type = uncasted_gdal_raster_view<T, IterationType, AccessType>;


    public:
      static const bool is_single_pass = IterationType == iteration_type::single_pass;
      static const bool is_mutable = AccessType != access::read_only;
      using value_type = T;

      uncasted_gdal_raster_iterator()
        : m_block()
        , m_end_of_stretch(block_iterator_type{})
        , m_pos(block_iterator_type{})
        //      , m_stride(0)
      {}

      uncasted_gdal_raster_iterator(const uncasted_gdal_raster_iterator&) = default;
      uncasted_gdal_raster_iterator(uncasted_gdal_raster_iterator&&) = default;
      uncasted_gdal_raster_iterator& operator=(const uncasted_gdal_raster_iterator&) = default;
      uncasted_gdal_raster_iterator& operator=(uncasted_gdal_raster_iterator&&) = default;
      ~uncasted_gdal_raster_iterator() = default;


      auto dereference() const {
        if constexpr (is_mutable)
        {
          if constexpr (is_single_pass)
          {
            return put_get_proxy_reference<const uncasted_gdal_raster_iterator&>(*this);
          }
          else {
            return put_get_proxy_reference<uncasted_gdal_raster_iterator>(*this);
          }
        }
        else {
          return get();
        }
      }

      void increment() {
        ++m_pos;

        if (m_pos == m_end_of_stretch) {
          --m_pos;
          goto_index(get_index() + 1);
        }
      }
      void decrement() {
        auto d = std::distance(m_block.get_iterator(0, 0), m_pos);
        if (d % m_block.block_cols() > 0) {
          --m_pos;
        }
        else
        {
          goto_index(get_index() - 1);
        }
      }

      void advance(std::ptrdiff_t offset) {
        goto_index(get_index() + offset);
      }

      bool equal_to(const uncasted_gdal_raster_iterator& other) const {
        return m_pos == other.m_pos;
      }

      std::ptrdiff_t distance_to(const uncasted_gdal_raster_iterator& other) const {
        return other.get_index() - get_index();
      }
    private:
      friend class put_get_proxy_reference<const uncasted_gdal_raster_iterator&>;
      friend class put_get_proxy_reference<uncasted_gdal_raster_iterator>;

      T get() const
      {
        return *static_cast<T*>(m_pos);
      }

      void put(const T& value) const
        requires (AccessType != access::read_only)
      {
        *static_cast<T*>(m_pos) = value;
      }

    private:
      friend class uncasted_gdal_raster_view<T, IterationType, AccessType>;

      void find_begin(const view_type* view)
      {
        m_view = view;
        goto_index(0);
      }

      void find_end(const view_type* view)
      {
        m_view = view;
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

        block_iterator_type start = m_block.get_iterator(0, 0);

        int index_in_block = static_cast<int>(std::distance(start, m_pos)); //  not -1 because m_pos has not been incremented
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

          index = static_cast<long long>(m_view->rows()) * static_cast<long long>(m_view->cols());
        }
        else {
          index = static_cast<long long>(row) * static_cast<long long> (m_view->cols()) + static_cast<long long>(col);
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
            ++m_pos;
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

          m_pos = m_block.get_iterator(row_in_block, col_in_block);

          int end_col = std::min<int>((block_col + 1) * block_cols
            , m_view->m_first_col + m_view->m_cols);

          int minor_end_col = 1 + (end_col - 1) % block_cols;

          m_end_of_stretch = m_block.get_iterator(row_in_block, minor_end_col);
        }
      }

      const view_type* m_view;
      block_type m_block;
      block_iterator_type m_end_of_stretch;
      block_iterator_type m_pos;
    };



    template<class T, iteration_type IterationType = iteration_type::multi_pass, access AccessType = access::read_write>
    class uncasted_gdal_raster_view : public std::ranges::view_interface<uncasted_gdal_raster_view<T, IterationType> >, public gdal_raster_view_base
    {
    private:
   
    public:
      using value_type = T;

      uncasted_gdal_raster_view(std::shared_ptr<GDALRasterBand> band)
        : m_band(band), m_rows(band->GetYSize()), m_cols(band->GetXSize()), m_first_row(0), m_first_col(0)
      {
        GDALDataType datatype = m_band->GetRasterDataType();
        assert(datatype == gdal_data_type<value_type>); //GDALDataType must be consistent with value_type;
        assert(GDALGetDataTypeSize(datatype) / 8 == sizeof(value_type)); //GDALDataType must be consistent with value_type;
        assert(m_band->GetAccess() != GA_ReadOnly || AccessType != access::read_write);// Don't have write access for read only dataset
      }
      uncasted_gdal_raster_view() = default;

      // using the aliasing constructor seems overly complicated now. Just remove for a
      // deleter that does nothing?
      uncasted_gdal_raster_view(GDALRasterBand* band)
        : uncasted_gdal_raster_view(std::shared_ptr<GDALRasterBand>{
        std::shared_ptr<GDALRasterBand>{}, band})
      {};

        using iterator = uncasted_gdal_raster_iterator<value_type, IterationType, AccessType>;

        std::shared_ptr<GDALRasterBand> get_band() const
        {
          return m_band;
        }
        //  friend create_standard_gdaldataset_from_model

        CPLErr get_geo_transform(double* padfTransform) const
        {
          CPLErr err = m_band->GetDataset()->GetGeoTransform(padfTransform);

          // Set this default affine transformation to be consistent with ARCGIS
          // For a raster with missing transformation, arcgis centers the top
          // left cell at (0,0). The cell-size is 1, the positive y-directions
          // direction is South to North.
          //
          if (err == CE_Failure) {
            padfTransform[0] = -0.5;
            padfTransform[1] = 1;
            padfTransform[2] = 0;
            padfTransform[3] = 0.5;
            padfTransform[4] = 0;
            padfTransform[5] = -1;
          }

          // Modify the transform such that is only for the sub-raster and not for
          // the dataset.
          padfTransform[0] = padfTransform[0]
            + padfTransform[1] * m_first_col
            + padfTransform[2] * m_first_row;

          padfTransform[3] = padfTransform[3]
            + padfTransform[4] * m_first_col
            + padfTransform[5] * m_first_row;

          return err;
        }

        int rows() const
        {
          return m_rows;
        };

        int cols() const
        {
          return m_cols;
        };

        int size() const
        {
          return rows() * cols();
        };

        uncasted_gdal_raster_view sub_raster(int first_row, int first_col, int rows, int cols) const
        {
          uncasted_gdal_raster_view out{ m_band };
          out.m_first_row = m_first_row + first_row;
          out.m_first_col = m_first_col + first_col;
          out.m_rows = rows;
          out.m_cols = cols;
          return out;
        }

        iterator begin() const
        {
          iterator i;
          i.find_begin(this);
          return i;
        }

        iterator end() const
        {
          iterator i;
          i.find_end(this);
          return i;
        }

        std::optional<T> get_nodata_value() const
        {
          int* check = nullptr;
          double value = m_band->GetNoDataValue(check);
          if (check) return std::optional<T>{static_cast<T>(value)};
          return std::optional<T>{};
        }

        void set_nodata_value(bool has_nodata, const T& value) const
        {
          if (has_nodata) m_band->SetNoDataValue(value);
          else m_band->DeleteNoDataValue();
        }

    private:
      //friend class iterator;
      //friend class const_iterator;
      friend class uncasted_gdal_raster_iterator<value_type, IterationType, AccessType>;
      std::shared_ptr<GDALRasterBand> m_band;
      int m_rows;
      int m_cols;
      int m_first_row;
      int m_first_col;
    };


    template<iteration_type IterationType = iteration_type::multi_pass, access AccessType = access::read_write>
    class gdal_raster_variant
    {
    public:
      static const bool is_raster_variant = true;

      gdal_raster_variant() = default;
      ~gdal_raster_variant() = default;

      template<class T>
      gdal_raster_variant(uncasted_gdal_raster_view <T, IterationType, AccessType> raster)
      {
        m_raster = raster;
      }

      template<class T>
      gdal_raster_variant(nodata_transformed_view<uncasted_gdal_raster_view <T, IterationType, AccessType>> raster)
      {
        m_raster = raster;
      }

      template<class T>
      using plain_type = uncasted_gdal_raster_view<T, IterationType, AccessType>;

      template<class T>
      using nodata_type = nodata_transformed_view<plain_type<T> >;


      std::variant<
        plain_type<uint8_t>,
        plain_type<int16_t>,
        plain_type<uint16_t>,
        plain_type<int32_t>,
        plain_type<uint32_t>,
        plain_type< float>,
        plain_type< double>,
        nodata_type< uint8_t>,
        nodata_type< int16_t>,
        nodata_type< uint16_t>,
        nodata_type< int32_t>,
        nodata_type< uint32_t>,
        nodata_type< float>,
        nodata_type< double> > m_raster;
    };
    
    /* 
    template<class Orig, class T, access A, iteration_type I>
    class cast_iterator : public iterator_facade<cast_iterator<Orig, T, A, I>>
    {
    public:
      static const bool is_mutable = A != access::read_only;
      static const bool is_single_pass = I == iteration_type::single_pass;

      void increment()
      {
        ++m_iter;
      }

      void advance(const std::ptrdiff_t& n)
      {
        m_iter += n;
      }

      void decrement()
      {
        --m_iter;
      }


      bool equal_to(const cast_iterator& that) const
      {
        return that.m_iter == m_iter;
      }

      std::ptrdiff_t distance_to(const cast_iterator& that) const
      {
        return that.m_iter - m_iter;
      }

      const T& dereference() const
      {
        if constexpr (is_mutable)
        {
          if constexpr (is_single_pass)
          {
            return put_get_proxy_reference<const cast_iterator&>(*this);
          }
          else {
            return put_get_proxy_reference<cast_iterator>(*this);
          }
        }
        else {
          return get();
        }
      }

      T get() const
      {
        return static_cast<T>(*m_iter);
      }
      void put(const T& t) 
      {
        *m_iter = static_cast<Orig::value_type>(t);
      }

      Orig m_iter;

    };

    template<class Orig, class T, access A, iteration_type I>
    class cast_raster_view : public std::ranges::view_interface<cast_raster_view<Orig, T, A, I>>
    {
      static const bool is_mutable = A != access::read_only;
      static const bool is_single_pass = I == iteration_type::single_pass;

      auto begin() const
      {
        cast_iterator<Orig::iterator, T, A, I> i{};
        i.m_iter = m_raster.begin();
        return i;

      }
      auto end() const
      {
        cast_iterator<Orig::iterator, T, A, I> i{};
        i.m_iter = m_raster.begin();
        return i;

      }
      int rows() const
      {
        return m_raster.rows();
      }

      int cols() const
      {
        return m_raster.cols();
      }
      int size() const
      {
        return m_raster.size();
      }
      using sub_raster_type = cast_raster_view<typename traits<Orig>::sub_raster, T, A, I>;
      sub_raster_type sub_raster(int from_row, int from_col, int rows, int cols)
      {
        sub_raster_type r;
        r.m_orig = m_raster.sub_raster(from_row, from_col, rows, cols);
        return r;
      }

      Orig m_raster;

    };

    template<class T, class R, access A = access::read_write, iteration_type I = iteration_type::multi_pass>
    auto cast_raster(R r)
    {
      auto cr = cast_raster_view<R, T, A, I>{};
      cr.m_raster = r;
      return cr;
    }
    */
  }
}
