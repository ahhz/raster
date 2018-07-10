//=======================================================================
// Copyright 2015-2107
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
// 

#pragma once
#include <pronto/raster/cached_block_raster_view.h>
#include <pronto/raster/io.h>
#include <pronto/raster/lru.h>
#include <pronto/raster/gdal_includes.h>

#include <memory>
#include <random> 
#include <utility> 
#include <vector> 

namespace pronto
{
  namespace raster
  {

    template<class T, class DataT> // todo overload special case where T == DataT
    struct gdal_read_write
    {
      static void read(GDALRasterBand* band, std::vector<T>& target, int block_index)
      {
        int rows_per_block;
        int cols_per_block;

        band->GetBlockSize(&cols_per_block, &rows_per_block);
      
        int rows = band->GetYSize();
        int cols = band->GetXSize();
       
        std::vector<DataT> original(rows_per_block * cols_per_block);
        int blocks_per_row = 1 + (cols - 1) / cols_per_block;
        int major_row = block_index / blocks_per_row;
        int major_col = block_index % blocks_per_row;

        band->ReadBlock(major_col, major_row, static_cast<void*>(&original[0]));
        target.assign(original.begin(), original.end());

      }

      static void write(GDALRasterBand* band, const std::vector<T>& target, int block_index)
      {
        int rows_per_block;
        int cols_per_block;

        band->GetBlockSize(&cols_per_block, &rows_per_block);

        int rows = band->GetYSize();
        int cols = band->GetXSize();

        std::vector<DataT> original;

        original.assign(target.begin(), target.end());

        int blocks_per_row = 1 + (cols - 1) / cols_per_block;
        int major_row = block_index / blocks_per_row;
        int major_col = block_index % blocks_per_row;

        band->WriteBlock(major_col, major_row, static_cast<void*>(&original[0]));
      }
    };

    template<class T, bool IsMutable>
    class gdal_block_provider
    {
    public:
      using value_type = T;
      using block = data_block<value_type>;

      gdal_block_provider(std::shared_ptr<GDALRasterBand> band)
        : m_rows(band->GetYSize()), m_cols(band->GetXSize()), m_band(band)
      {
        band->GetBlockSize(&m_block_cols, &m_block_rows);
        int number_of_blocks_per_row = 1 + (m_rows -1) / m_block_rows;
        int number_of_blocks_per_col = 1 + (m_cols - 1)/ m_block_cols;
        int number_of_blocks = number_of_blocks_per_row * 
          number_of_blocks_per_col;
        m_blocks.resize(number_of_blocks);

              // Using pointers to member functions as a means of run-time polymorphism
        static_assert(sizeof(float) == 4, "GDAL assumes size of float is 4 bytes");
        static_assert(sizeof(double) == 8, "GDAL assumes size of double is 8 bytes");
        switch (m_band->GetRasterDataType())
        {
        case GDT_Byte:     set_io<uint8_t >();   break;
        case GDT_Int16:    set_io<int16_t >();   break;
        case GDT_UInt16:   set_io<uint16_t>();   break;
        case GDT_Int32:    set_io<int32_t>();    break;
        case GDT_UInt32:   set_io<uint32_t>();   break;
        case GDT_Float32:  set_io<float>();      break;
        case GDT_Float64:  set_io<double>();     break;
          // Complex numbers not currently supported 
          //
          //case GDT_CInt16:   set_io<cint16_t>();   break;
          //case GDT_CInt32:   set_io<cint32_t>();   break;
          //case GDT_CFloat32: set_io<cfloat32_t>(); break;
          //case GDT_CFloat64: set_io<cfloat64_t>(); break;
        default: break;
        }
        if (m_band->GetAccess() == GA_ReadOnly) {
         
        }
      }

      template<class DataT>
      void set_io()
      {
        m_read_block = gdal_read_write<T, DataT>::read;
        m_write_block = gdal_read_write<T, DataT>::write;
      }

      int rows() const
      {
        return m_rows;
      }

      int cols() const
      {
        return m_cols;
      }

      int block_rows() const // the number of rows in a block
      {
        return m_block_rows;
      }

      int block_cols() const
      {
        return m_block_cols;
      }

      block* get_block(int index)
      {
        block& b = m_blocks[index];
        if (b.has_data()) {

          b.touch();
        }
        else {

          b.create_data(m_block_rows * m_block_cols);
          m_read_block(m_band.get(), b.get_vector(), index);
          auto writer = [index, &b, this]() {m_write_block(m_band.get(), b.get_vector(), index); };
          b.set_pre_clear(writer);
        }
        return &(b);
      }
    private:
      std::function<void(GDALRasterBand*, std::vector<T>&, int)> m_read_block;
      std::function<void(GDALRasterBand*, const std::vector<T>&, int)> m_write_block;
      std::shared_ptr<GDALRasterBand> m_band;
      std::vector< data_block<T> > m_blocks;
      int m_rows;
      int m_cols;
      int m_block_rows;
      int m_block_cols;
    };



    template<class ValueType, bool IsForwardOnly, bool IsMutable>
      using gdal_raster_view_v2 = cached_block_raster_view<
        gdal_block_provider<ValueType, IsMutable>
      , IsMutable 
      , IsForwardOnly
      >;

    
     template<class ValueType, bool IsForwardOnly, bool IsMutable>
     gdal_raster_view_v2<ValueType, IsForwardOnly, IsMutable>
      open_v2(const filesystem::path& path)
    {
       access elem_access = IsMutable ? access::read_write : access::read_only;
       auto dataset = detail::open_dataset(path, elem_access);
       auto band = detail::open_band(dataset, 1);
       using provider = gdal_block_provider<ValueType, IsMutable>;
       std::shared_ptr<provider> gbp(new provider(band));
       return gdal_raster_view_v2<ValueType, IsForwardOnly, IsMutable>
         (gbp);
    }

     template<class ValueType, bool IsForwardOnly = false>
     gdal_raster_view_v2<ValueType, IsForwardOnly, true>
       create_v2(
       const filesystem::path& path, int rows, int cols
       , GDALDataType data_type = detail::native_gdal_data_type<ValueType>::value)
     {
       if (data_type == GDT_Unknown)
       {
         throw(creating_a_raster_failed{});
       }

       std::shared_ptr<GDALRasterBand> band = detail::create_band
       (path, rows, cols, data_type, is_temporary::no);
       static const bool is_mutable = true;
       using provider = gdal_block_provider<ValueType, is_mutable>;
       std::shared_ptr<provider> gbp(new provider(band));
       return gdal_raster_view_v2<ValueType, IsForwardOnly, is_mutable>(gbp);
     }
  }
}
