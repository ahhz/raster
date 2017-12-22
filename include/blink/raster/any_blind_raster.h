//
//=======================================================================
// Copyright 2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once

#include <blink/raster/any.h>
#include <blink/raster/any_raster.h>

#include <functional>
#include <vector>
 
#pragma warning( push )  
#pragma warning( disable : 4244 )  // Suppressing warning related to casting,
#pragma warning( disable : 4267 )  // these are inherent to any_blind_raster

namespace blink {
  namespace raster {

    using gdal_data_types = std::tuple
      < uint8_t
      , int16_t
      , uint16_t
      , int32_t
      , uint32_t
      , float
      , double
      //, cint16_t
      //, cint32_t
      //, cfloat32_t
      //, cfloat64_t
      >;

    template<class T>
    struct gdal_class
    {
      using type = T;
      static const std::size_t index 
        = index_in_tuple<T, gdal_data_types>::value; // static asserts
    };

    template<>
    struct gdal_class<bool>
    {
      using type = uint8_t;
    };

    using blind_data_types = std::tuple
      < bool
      , uint8_t
      , int16_t
      , uint16_t
      , int32_t
      , uint32_t
      , float
      , double
      //, cint16_t
      //, cint32_t
      //, cfloat32_t
      //, cfloat64_t
      >;
    

    namespace detail {
      template <class T, class Tuple>
      struct Index;

      template <class T>
      struct Index<T, std::tuple<> > {
        static const std::size_t value = 0;
      };

      template <class T, class... Types>
      struct Index<T, std::tuple<T, Types...>> {
        static const std::size_t value = 0;
      };

      template <class T, class U, class... Types>
      struct Index<T, std::tuple<U, Types...>> {
        static const std::size_t value = 1 + Index<T, std::tuple<Types...>>::value;
      };
    }

    template<class T, class Tuple>
    struct index_in_tuple
    {
      static const std::size_t value = detail::Index<T, Tuple>::value;
      static_assert(value != std::tuple_size<Tuple>::value, "No T in Tuple");
    };
      
    class any_blind_raster
    {
      template<std::size_t N>
      using value_type = typename std::tuple_element<N, blind_data_types>::type;

    public:
      any_blind_raster() = default;

      template<typename T>
      any_blind_raster(const any_raster<T>& data) 
        : m_data(data)
        , m_index(index_in_tuple<T, blind_data_types>::value)
      {
        m_get_rows = [&]() {return get_by_type<T>().rows(); };
        m_get_cols = [&]() {return get_by_type<T>().cols(); };
        m_get_size = [&]() {return get_by_type<T>().size(); };
        m_get_sub_raster = [&](int a, int b, int c, int d) {
          return make_any_raster(get_by_type<T>().sub_raster(a, b, c, d));
        };
      }

      template<std::size_t Index>
      any_raster < value_type<Index> >
        get_by_index() const// will throw if you specify the wrong Index
      {
         return any_cast< any_raster<value_type<Index> > >(m_data);
      }

      template<class T>
      any_raster<T> get_by_type() // will throw if you specify the wrong T
      {
        return any_cast<any_raster<T> >(m_data);
      }

      int rows() const 
      { 
        return m_get_rows();
      }

      int cols() const 
      {
        return m_get_cols();
      }

      int size() const
      {
        return m_get_size();
      }

      std::size_t index() const
      {
        return m_index;
      }

      any_blind_raster sub_raster(int r, int c, int rs, int cs) const
      {
        return m_get_sub_raster(r,c,rs,cs);
      }

    private:
      std::size_t m_index;
      any m_data; 

      std::function<int()> m_get_rows;
      std::function<int()> m_get_cols;
      std::function<int()> m_get_size;
      std::function<any_blind_raster(int, int, int, int)> m_get_sub_raster;
    };

    template<class Raster>
    any_blind_raster make_any_blind_raster(Raster r)
    {
      return any_blind_raster(make_any_raster(r));
    }


  }
}
#pragma warning( pop )