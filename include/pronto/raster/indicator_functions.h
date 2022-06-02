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

#include <pronto/raster/transform_raster_view.h>
#include <pronto/raster/weighted_raster_view.h>
#include <pronto/raster/optional.h>

namespace pronto {
  namespace raster {
    /*
    template<class Raster, class IndicatorTag>
    struct get_indicator
    {
      using value_type = typename traits<Raster>::value_type;
      using type = typename indicator_traits<IndicatorTag,
        recursive_optional_value_type<value_type> >::type;
    };

    template<class Raster, class IndicatorTag>
    using get_indicator_t = typename get_indicator<Raster, IndicatorTag>::type;
    */
    template<class Indicator, class Value>
    struct indicator_functions
    {
      static void add(Indicator& indicator, const Value& value)
      {
        if (recursive_is_initialized(value) ){
          indicator.add_sample(recursive_get_value(value));
        }
      }
      static void subtract(Indicator& indicator, const Value& value)
      {
        if (recursive_is_initialized(value) ){
          indicator.subtract_sample(recursive_get_value(value));
        }
      }
    };

    template<class Indicator, class Value, class Weight>
    struct indicator_functions<Indicator, weighted_value<Value, Weight> >
    {
      static void add(Indicator& indicator, const weighted_value<Value, Weight>& wv)
      {
        if (recursive_is_initialized(wv.m_value) 
          && recursive_is_initialized(wv.m_weight)) {
          indicator.add_sample(recursive_get_value(wv.m_value)
            , recursive_get_value(wv.m_weight));
        }
      }

      static void subtract(Indicator& indicator, const weighted_value<Value, Weight>& wv)
      {
        if (recursive_is_initialized(wv.m_value)
          && recursive_is_initialized(wv.m_weight) ) {
          indicator.subtract_sample(recursive_get_value(wv.m_value),
            recursive_get_value(wv.m_weight));
        }
      };
    };

    struct indicator_joiner
    {
      template<class Indicator>
      Indicator operator()(Indicator a, const Indicator& b) const
      {
        a.add_subtotal(b);
        return a;
      }
    };

    struct indicator_extractor
    {
      template<class Indicator>
      auto operator()(const Indicator& i) const
        ->decltype(i.extract())
      {
        return i.extract();
      }
    };

    //template<class IndicatorView>
    //using extracted_indicator_view = transform_raster_view<indicator_extractor,
    //  IndicatorView>;

    template<class IndicatorView>
    auto extract(IndicatorView iv)
    {
      return transform(indicator_extractor{}, iv);
    }
  }
}
