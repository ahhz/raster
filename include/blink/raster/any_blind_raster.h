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
#include <blink/raster/complex_numbers.h>
#include <blink/raster/filesystem.h>
#include <blink/raster/transform_raster_view.h>

#include <functional>
#include <vector>

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
      static const std::size_t index = index_in_tuple<T, gdal_data_types>::value; // static asserts
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
    
    class any_blind_raster;

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

    struct row_getter
    {
        template<class T> int operator()(const any_raster<T>& r)
        {
          return r.rows();
        }
    };

    struct col_getter
    {
      template<class T> int operator()(const any_raster<T>& r)
      {
        return r.cols();
      }
    };
    
    struct size_getter
    {
      template<class T> int operator()(const any_raster<T>& r)
      {
        return r.size();
      }
    };

    struct sub_raster_getter
    {
      sub_raster_getter(int row, int col, int rows, int cols)
        : m_from_row(row), m_from_col(col), m_rows(rows), m_cols(cols)
      {}

      template<class T> any_blind_raster operator()(any_raster<T> r)
      {
        return any_blind_raster(
          r.sub_raster(m_from_row, m_from_col, m_rows, m_cols));
      }

      int m_from_row;
      int m_from_col;
      int m_rows;
      int m_cols;
    };

    template<class F>
    using apply_on_any_blind_raster_return_type = decltype(std::declval<F>()(any_raster<int>{}));

    template<class F, class T>
    apply_on_any_blind_raster_return_type<F>
      apply_function_on_typed_blind_raster(F f, any_blind_raster r)
    {
      return f(r.get_by_type<T>());
    }

    template<class, class>  struct make_functions;
    template<class F, class... T>
    struct make_functions<F, std::tuple<T...>>
    {
      using return_type = apply_on_any_blind_raster_return_type<F>;
      std::vector<std::function<return_type(F, any_blind_raster) > >operator()(F f, any_blind_raster r)
      {
        return std::vector<std::function<return_type(F, any_blind_raster)> >{
          std::bind(apply_function_on_typed_blind_raster<F, T>
            , std::placeholders::_1, std::placeholders::_2)...};
      }
    };

    template<class F>
    apply_on_any_blind_raster_return_type<F> blind_function(F f, any_blind_raster r)
    {
      using return_type = apply_on_any_blind_raster_return_type<F>;
      std::vector<std::function<return_type(F, any_blind_raster)> > functions
        = make_functions<F, blind_data_types>{}(f, r);
  
      return functions[r.index()](f, r);
    }
     
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
      }

      template<std::size_t Index>
      any_raster < value_type<Index> >
        get_by_index() const// will throw if you specify the wrong Index
      {
         return any_cast< any_raster <value_type<Index>> >(m_data);
      }

	    template<class F>
      apply_on_any_blind_raster_return_type<F> apply(F function) const
	    {
        return blind_function(function, *this);
      }

      template<class T>
      any_raster <T> get_by_type() // will throw if you specify the wrong T
      {
        return  any_cast<any_raster<T> >(m_data);
      }

      int rows() const 
      { 
        return apply(row_getter{});
      }

      int cols() const 
      {
        return apply(col_getter{});
      }

      int size() const
      {
        return apply(size_getter{});
      }

      std::size_t index() const
      {
        return m_index;
      }

      any_blind_raster sub_raster(int r, int c, int rs, int cs) const
      {
         return apply(sub_raster_getter{r, c, rs, cs});
      }

    private:
      std::size_t m_index;
      any m_data; 
    };

    template<class Raster>
    any_blind_raster make_any_blind_raster(Raster r)
    {
      return any_blind_raster(make_any_raster(r));
    }

    template<class F, class Raster>
    struct apply_first
    {
      apply_first(F f, Raster r2) :m_f(f), m_r2(r2)
      {}

      template<class T>
      any_blind_raster operator()(any_raster<T> r1)
      {
        return make_any_blind_raster(transform(m_f, r1, m_r2));
      }

      F m_f;
      Raster m_r2;
    };

    template<class F, class Raster>
    struct apply_second
    {
      apply_second(F f, Raster r1) :m_f(f), m_r1(r1)
      {}

      template<class T>
      any_blind_raster operator()(any_raster<T> r2)
      {
        return make_any_blind_raster(transform(m_f, m_r1, r2));
      }

      F m_f;
      Raster m_r1;
    };

    template<class F>
    struct apply_first_and_second
    {
      apply_first_and_second(F f, any_blind_raster r1) :m_f(f), m_r1(r1)
      {}

      template<class T>
      any_blind_raster operator()(any_raster<T> r2)
      {
        return blind_function(apply_first<F, any_raster<T> >{ m_f, r2 }, m_r1);
      }

      F m_f;
      any_blind_raster m_r1;
    };

    template<class F>
    struct apply_unary
    {
      apply_unary(F function) : m_function(function)
      {}

      template<class T>
      any_blind_raster operator()(any_raster<T> r)
      {
        return make_any_blind_raster(transform(m_function, r));
      }
      F m_function;
    };
  }
}
