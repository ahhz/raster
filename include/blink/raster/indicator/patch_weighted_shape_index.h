
//=======================================================================
// Copyright 2015
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================

#pragma once

#include <blink/raster/optional.h>
#include <blink/raster/patch_raster_transform.h>

#include <cmath> //sqrt

namespace blink {
  namespace raster {

    // As implemented, the shape index is recalculated every time it is used
    // it may be more efficient to keep values in a vector / cache
    // TODO: need to be more sensible about data types for sum and weight. 
    // Normally these are floating point, but weight can also be integer.

    template<class Weight>
    struct patch_weighted_shape_index
    {
      using weight_type = Weight;

      double calculate_shape_index(const patch_info& p)
      {
        int n = static_cast<int>(std::sqrt(p.m_area));
        int min_perimeter;
        if (p.m_area == n * n) {
          min_perimeter = 4 * n;
        }
        else if (p.m_area < n * (n + 1)) {
          min_perimeter = 4 * n + 2;
        }
        else {
          min_perimeter = 4 * n + 4;
        }
        return static_cast<double>(p.m_perimeter) / min_perimeter;
      }

      void add_sample(const patch_info& i)
      {
        const double shape = calculate_shape_index(i);
        double w = 1.0 / i.m_area;
        m_weight += w;
        m_sum += w * shape;
      }

      void add_sample(const patch_info& i, const weight_type& w)
      {
        const double shape = calculate_shape_index(i);
        double w2 = w  * 1.0 / i.m_area;
        m_weight += w2;
        m_sum += w2 * shape;
      }

      void subtract_sample(const patch_info& i)
      {
        const double shape = calculate_shape_index(i);
        const double w = 1.0 / i.m_area;
        m_weight -= w;
        m_sum -= w * shape;
      }

      void subtract_sample(const patch_info& i, const weight_type& w)
      {
        const double shape = calculate_shape_index(i);
        const double w2 = w  * 1.0 / i.m_area;
        m_weight -= w2;
        m_sum -= w2 * shape;
      }

      void add_subtotal(const patch_weighted_shape_index& subtotal)
      {
        m_sum += subtotal.m_sum;
        m_weight += subtotal.m_weight;
      }

      void add_subtotal(const patch_weighted_shape_index& subtotal, const weight_type& w)
      {
        m_sum += w * subtotal.m_sum;
        m_weight += w * subtotal.m_weight;
      }

      void subtract_subtotal(const patch_weighted_shape_index& subtotal)
      {
        m_sum -= subtotal.m_sum;
        m_weight -= subtotal.m_weight;
      }

      void subtract_subtotal(const patch_weighted_shape_index& subtotal, const weight_type& w)
      {
        m_sum -= w * subtotal.m_sum;
        m_weight -= w * subtotal.m_weight;
      }

      optional<double> extract() const
      {
        if (m_weight > 1e-8) return m_sum;
        return m_sum / m_weight;
      }

      double m_sum;
      double m_weight;
    };
    struct patch_weighted_shape_index_generator
    {
      using indicator = patch_weighted_shape_index<double>;

      indicator operator()() const
      {
        return indicator{};
      }
    };
  }
}
