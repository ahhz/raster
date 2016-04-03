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
// efficiently accessible through iterators. It uses its own disk caching 
// and not gdal's. 
//

#ifndef BLINK_RASTER_GDAL_RASTER_H_AHZ
#define BLINK_RASTER_GDAL_RASTER_H_AHZ

//#include <moving_window/coordinate_2d.h>
//#include <moving_window/exceptions.h>
#include <blink/raster/coordinate_2d.h>
#include <blink/raster/default_raster_view.h>
#include <blink/raster/gdal_raster_lru.h>
#include <blink/raster/gdal_raster_iterator.h>
#include <blink/raster/raster_iterator.h>
#include <blink/raster/raster_traits.h>

#include <cpl_string.h> // part of GDAL
#include <gdal.h>       // part of GDAL

#pragma warning( push )
#pragma warning( disable : 4251 )
#include <gdal_priv.h>  // part of GDAL
#pragma warning( pop ) 

#include <boost/filesystem.hpp> 
#include <utility>

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
        , m_major_size2(0)
        , m_block_size1(1), m_block_size2(1)
        , m_access_type(GA_ReadOnly)
        , m_file_path("blank")
        , m_delete_on_close(false)
        , m_gdal_dataset(NULL)
        , m_gdal_rasterband(NULL)
        , m_cache(NULL)
      {}

    public:
   //   friend class detail::gdal_makers;

      // Constructor opens from file
      //
      gdal_raster(const boost::filesystem::path& file_path, GDALAccess access,
        int band = 1)
        : m_access_type(access), m_delete_on_close(false), m_file_path(file_path),
        m_band(band)
      {
        GDALAllRegister();
        m_gdal_dataset = (GDALDataset *)GDALOpen(m_file_path.string().c_str()
          , m_access_type);

        if (m_gdal_dataset == NULL) {
          std::cout << "Could not read: " << m_file_path.c_str() << std::endl;
          BOOST_THROW_EXCEPTION(opening_raster_failed{});
        }
        initialize();
      }
 
      // Constructor opens from opened GDALDataset
      //
      gdal_raster(GDALDataset * dataset, const boost::filesystem::path& file_path,
        int band = 1)
        : m_delete_on_close(false), m_file_path(file_path), m_gdal_dataset(dataset),
        m_band(band)
      {
        m_access_type = dataset->GetAccess();
        initialize();
      }

      template<class Range>
      gdal_raster& operator=(Range& r)
      {
        auto i = r.begin();
        auto j = begin();
        auto j_end = end();
        for (; j != j_end; ++i, ++j)
        {
          *j = *i;
        }
        return *this;
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
      // Move constructor
      //
      gdal_raster(gdal_raster<T>&& other)
      {
        m_band = other.m_band;
        m_major_size2 = other.m_major_size2;
        m_block_size1 = other.m_block_size1;
        m_block_size2 = other.m_block_size2;
        m_access_type = other.m_access_type;
        m_delete_on_close = other.m_delete_on_close;
        m_file_path = other.m_file_path;
        m_gdal_dataset = other.m_gdal_dataset;
        m_gdal_rasterband = other.m_gdal_rasterband;
        m_cache = other.m_cache;
        put_pixel = other.put_pixel;
        get_pixel = other.get_pixel;

        // Release the data pointer from the source object so that
        // the destructor does not free the memory multiple times.
        other.m_band = 0;
        other.m_major_size2 = 0;
        other.m_block_size1 = 0;
        other.m_block_size2 = 0;
        //other.m_access_type = 0;
        other.m_delete_on_close = false;
        other.m_file_path = "";
        other.m_gdal_dataset = NULL;
        other.m_gdal_rasterband = NULL;
        other.m_cache = NULL;
      }

      // Move assignment 
      //
      gdal_raster& operator=(gdal_raster<T>&& other)
      {
        close_dataset();
        m_band = other.m_band;
        m_major_size2 = other.m_major_size2;
        m_block_size1 = other.m_block_size1;
        m_block_size2 = other.m_block_size2;
        m_access_type = other.m_access_type;
        m_delete_on_close = other.m_delete_on_close;
        m_file_path = other.m_file_path;
        m_gdal_dataset = other.m_gdal_dataset;
        m_gdal_rasterband = other.m_gdal_rasterband;
        m_cache = other.m_cache;
        put_pixel = other.put_pixel;
        get_pixel = other.get_pixel;

        // Release the data pointer from the source object so that
        // the destructor does not free the memory multiple times.
        other.m_band = 0;
        other.m_major_size2 = 0;
        other.m_block_size1 = 0;
        other.m_block_size2 = 0;
        //other.m_access_type = 0;
        other.m_delete_on_close = false;
        other.m_file_path = "";
        other.m_gdal_dataset = NULL;
        other.m_gdal_rasterband = NULL;
        other.m_cache = NULL;
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
        const void* buf = get_buf(get_major_index(coord));
        return get_pixel(buf, get_minor_index(coord));
      }
      
      void put(const coordinate_type& coord, const T& value)
      {
       void* buf = get_buf_for_writing(get_major_index(coord));
       return put_pixel(value, buf, get_minor_index(coord));
      }

//   private: // should be private??
//    template<class> friend class gdal_iterator;
//    template<class> friend class trans_gdal_iterator;
  
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
        const void* buf = get_buf(block);
        return get_pixel(buf, index_in_block);
      }

      void put_pixel_in_block(index_type block, index_type index_in_block,
        const T& value)
      {
        void* buf = get_buf_for_writing(block);
        return put_pixel(value, buf, index_in_block);
      }
      const GDALDataset* get_gdal_dataset() const
      {
        return m_gdal_dataset;
      }

    private:
      void initialize()
      {
        m_gdal_rasterband = m_gdal_dataset->GetRasterBand(static_cast<int>(m_band));

        // User our own memory management
        m_cache = new LRU::block_manager(m_gdal_rasterband);

        int x_size, y_size;

        m_gdal_rasterband->GetBlockSize(&x_size, &y_size);

        m_block_size1 = static_cast<index_type>(y_size);
        m_block_size2 = static_cast<index_type>(x_size);
        m_major_size2 = static_cast<index_type>(1 + (size2() - 1) / m_block_size2);

        // Using pointers to member functions as a means of run-time polymorphism
        switch (m_gdal_rasterband->GetRasterDataType())
        {
        case GDT_Byte:
          put_pixel = &this_type::put_pixel_specialized<unsigned char,
            sizeof(unsigned char)>;
          get_pixel = &this_type::get_pixel_specialized<unsigned char,
            sizeof(unsigned char)>;
          break;
        case GDT_Int16:
          put_pixel = &this_type::put_pixel_specialized<short, sizeof(short)>;
          get_pixel = &this_type::get_pixel_specialized<short, sizeof(short)>;
          break;
        case GDT_UInt16:
          put_pixel = &this_type::put_pixel_specialized<unsigned short,
            sizeof(unsigned short)>;
          get_pixel = &this_type::get_pixel_specialized<unsigned short,
            sizeof(unsigned short)>;
          break;
        case GDT_Int32:
          put_pixel = &this_type::put_pixel_specialized<int, sizeof(int)>;
          get_pixel = &this_type::get_pixel_specialized<int, sizeof(int)>;
          break;
        case GDT_UInt32:
          put_pixel = &this_type::put_pixel_specialized<unsigned int,
            sizeof(unsigned int)>;
          get_pixel = &this_type::get_pixel_specialized<unsigned int,
            sizeof(unsigned int)>;
          break;
        case GDT_Float32:
          put_pixel = &this_type::put_pixel_specialized<float, sizeof(float)>;
          get_pixel = &this_type::get_pixel_specialized<float, sizeof(float)>;
          break;
        case GDT_Float64:
          put_pixel = &this_type::put_pixel_specialized<double, sizeof(double)>;
          get_pixel = &this_type::get_pixel_specialized<double, sizeof(double)>;
          break;
        default: break;

        }
        if (m_access_type == GA_ReadOnly) {
          put_pixel = &this_type::put_nothing;
        }
      }


      void close_dataset()
      {
        delete m_cache; // flushes so must be done before calculating statistics
        m_cache = nullptr;
        if (m_gdal_dataset && m_access_type == GA_Update) {
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
              for (int i = 0; file_list[i] != 0; ++i) {
                boost::filesystem::path path(file_list[i]);
                boost::filesystem::remove(path);
              }
            }
          }
          CSLDestroy(file_list);
        }
      }
      boost::filesystem::path get_path() const
      {
        return m_file_path;
      }

      index_type get_major_index(const coordinate_type& coord) const
      {
        const index_type major_row = coord.row / m_block_size1;
        const index_type major_col = coord.col / m_block_size2;
        return major_row * m_major_size2 + major_col;
      }

      index_type get_minor_index(const coordinate_type& coord) const
      {
        const index_type minor_row = coord.row % m_block_size1;
        const index_type minor_col = coord.col % m_block_size2;
        return minor_row * m_block_size2 + minor_col;
      }

      const void* get_buf(const index_type& major_index) const
      {
        return m_cache->get(major_index);
      }

      void* get_buf_for_writing(const index_type& major_index)
      {
        return m_cache->get_for_writing(major_index);
      }

      template<typename TData, index_type stride>
      static void put_pixel_specialized(const value_type& value, void* const buf
        , index_type n)
      {
        char* const point_at_char = reinterpret_cast<char* const>(buf)+n*stride;
        TData* const point_at_data = reinterpret_cast<TData* const>(point_at_char);
        (*point_at_data) = static_cast<TData>(value);
      }

      template<typename TData, index_type stride>
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

      // function pointers for "runtime polymorphism" based on file datatype. 
      void(*put_pixel)(const value_type&, void* const, index_type);
      value_type(*get_pixel)(const void* const, index_type);

      int m_band;
      index_type m_major_size2;
      index_type m_block_size1;
      index_type m_block_size2;
      GDALAccess m_access_type;
      bool m_delete_on_close;
      boost::filesystem::path m_file_path;

      // mutable because not const - correctly implemented
      mutable GDALDataset* m_gdal_dataset;
      mutable GDALRasterBand* m_gdal_rasterband;

      LRU::block_manager* m_cache;
    };

 
  } //namespace 
}
#endif