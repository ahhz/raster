
//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================

#pragma once

#include <pronto/raster/traits.h>
#include <pronto/raster/optional.h>

namespace pronto {
  namespace raster {

    template<class SampleType, class WeightType = double>
    struct mean
    {
      mean() : weight(0), sum(0)
      {}

      using value_type = SampleType;
      using weight_type = WeightType;

      void add_sample(const value_type& v, const weight_type& w)
      {
        weight += w;
        sum += w * static_cast<double>(v);
      }

      void add_sample(const value_type& v)
      {
        weight++;
        sum += static_cast<double>(v);
      }

      void subtract_sample(const value_type& v, const weight_type& w)
      {
        weight -= w;
        sum -= w * static_cast<double>(v);
      }

      void subtract_sample(const value_type& v)
      {
        weight--;
        sum -= static_cast<double>(v);
      }

      void add_subtotal(const mean& subtotal, const weight_type& w)
      {
        weight += w * subtotal.weight;
        sum += w * subtotal.sum;
      }

      void add_subtotal(const mean& subtotal)
      {
        weight+= subtotal.weight;
        sum += subtotal.sum;
      }

      void subtract_subtotal(const mean& subtotal, const weight_type& w)
      {
        weight -= w * subtotal.weight;
        sum -= w * subtotal.sum;
      }

      void subtract_subtotal(const mean& subtotal)
      {
        weight-= subtotal.weight;
        sum -= subtotal.sum;
      }

      std::optional<double> extract() const
      {
        if (weight > 0) return std::optional<double>(sum / weight);
        return std::nullopt;
      }

      double weight;
      double sum;
    };

    template<class SampleType = double, class WeightType = double>
    class mean_generator
    {
    public:
      using indicator = mean<SampleType, WeightType>;
      indicator operator()() const
      {
        return indicator{};
      }
    };
  }
}
