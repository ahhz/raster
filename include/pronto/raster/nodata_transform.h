//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once

#include <pronto/raster/optional.h>
#include <pronto/raster/traits.h>
#include <pronto/raster/transform_raster_view.h>

namespace pronto
{
  namespace raster
  {
    template<class T> 
    class nodata_to_optional_functor
    {
    public:
      template<class T>
      nodata_to_optional_functor(const T& nodata_value) 
        : m_nodata_value(nodata_value)
      {}
      nodata_to_optional_functor() = default;
      ~nodata_to_optional_functor() = default;
      nodata_to_optional_functor(const nodata_to_optional_functor&) = default;
      nodata_to_optional_functor(nodata_to_optional_functor&&) = default;
      nodata_to_optional_functor& operator=(const nodata_to_optional_functor&) 
        = default;
      nodata_to_optional_functor& operator=(nodata_to_optional_functor&&) 
        = default;
     
      std::optional<T> operator()(const T& value) const
      {
        if (value == m_nodata_value) return std::nullopt;
        else return std::optional<T>(value);
      }

    private:
      T m_nodata_value;
    };

    template<class T>
    class optional_to_nodata_functor
    {
    public:
      optional_to_nodata_functor(const T& nodata_value) 
        : m_nodata_value(nodata_value)
      {}
      optional_to_nodata_functor() = default;
      ~optional_to_nodata_functor() = default;
      optional_to_nodata_functor(const optional_to_nodata_functor&) = default;
      optional_to_nodata_functor(optional_to_nodata_functor&&) = default;
      optional_to_nodata_functor& operator=(const optional_to_nodata_functor&) 
        = default;
      optional_to_nodata_functor& operator=(optional_to_nodata_functor&&) 
        = default;

      template<class OptionalT>
      T operator()(const OptionalT& value) const
      {
        if (recursive_is_initialized(value)) return recursive_get_value(value);
        return m_nodata_value;
      }

      T m_nodata_value;
    };
    
  //  template<class Raster>
  //  using nodata_to_optional_raster_view =
  //    transform_raster_view<nodata_to_optional_functor<
  //    typename traits<Raster>::value_type>, Raster>;
    
  //  template<class Raster>
  //  using optional_to_nodata_raster_view =
  //    transform_raster_view< optional_to_nodata_functor<
  //    recursive_optional_value_type<
  //    typename traits<Raster>::value_type > >, Raster>;


    template<class Raster>
    auto nodata_to_optional(
      Raster r, const typename traits<Raster>::value_type& nodata_value)
    {
      using value_type = typename traits<Raster>::value_type;
      return transform(nodata_to_optional_functor<value_type>(nodata_value), r);
    }

    template<class R>
    using nodata_transformed_view = transform_raster_view< nodata_to_optional_functor<typename traits<R>::value_type>, R>;

    template<class Raster>
    auto optional_to_nodata(
      Raster r, const recursive_optional_value_type<
        typename traits<Raster>::value_type >& nodata_value)
    {
      using value_type = recursive_optional_value_type<
        typename traits<Raster>::value_type >;
      using functor = optional_to_nodata_functor<value_type>;
      return transform(functor(nodata_value), r);
    }
  }
}
