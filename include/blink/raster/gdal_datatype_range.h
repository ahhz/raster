//
//=======================================================================
// Copyright 2016
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#ifndef BLINK_RASTER_GDAL_DATATYPE_RANGE_H_AHZ
#define BLINK_RASTER_GDAL_DATATYPE_RANGE_H_AHZ

#include <boost/any.hpp>
#include <typeinfo>

namespace blink {
  namespace raster {
    class any_any_input_range
    {
    public:
      using type_index = std::size_t;
      template<typename T>
      any_any_input_range(const any_input_range<T>& range)
        : m_range(range)
        , m_is_reference(false)
        , get_info(get_info_template<T>)
      {}

      template<typename T>
      any_any_input_range(
        const std::reference_wrapper<any_input_range<T> >& range)
        : m_range(range)
        , m_datatype(datatype_lookup<T>::value)
        , m_is_reference(true)
      {}


      template<typename U>
      bool check_type() const
      {
        return typeid(U) == get_info();
      };

      template<typename T>
      any_input_range<T>&  get_range()
      {
        if (is_reference)
        {
          auto ref = boost::any_cast<
            std::reference_wrapper<any_input_range<T> >>(m_range);
          return ref.get();
        }
        else {
          return boost::any_cast<any_input_range<int>&>(m_range);
        }
      }
      
    private:
      template<typename U>
      static const std::typeinfo& get_info_template()
      {
        return typeid(U);
      }

      const std::typeinfo& (*get_info)();
      boost::any m_range;
      bool m_is_reference;
    };
  }
}

#endif

