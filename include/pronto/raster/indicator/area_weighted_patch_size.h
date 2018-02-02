
//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================

#pragma once

#include <pronto/raster/optional.h>
#include <pronto/raster/patch_raster_transform.h>
#include <pronto/raster/traits.h>

namespace pronto { 
  namespace raster {

    template<class Weight = double>
    struct area_weighted_patch_size
    {
      area_weighted_patch_size() : m_sum(0), m_weight(0)
      {}

      void add_sample(const patch_info& i)
      {
        m_weight++;
        m_sum += i.m_area;
      }

      void add_sample(const patch_info& i, const Weight& w)
      {
        m_weight += w;
        m_sum += w * i.m_area;
      }

      void subtract_sample(const patch_info& i)
      {
        m_weight--;
        m_sum -= i.m_area;
      }

      void subtract_sample(const patch_info& i, const Weight& w)
      {
        m_weight -= w;
        m_sum -= w * i.m_area;
      }

      void add_subtotal(const area_weighted_patch_size& subtotal)
      {
        m_sum += subtotal.m_sum;
        m_weight += subtotal.m_weight;
      }

      void add_subtotal(const area_weighted_patch_size& subtotal, const Weight& w)
      {
        m_sum += w * subtotal.m_sum;
        m_weight += w * subtotal.m_weight;
      }

      void subtract_subtotal(const area_weighted_patch_size& subtotal)
      {
        m_sum -= subtotal.m_sum;
        m_weight -= subtotal.m_weight;
      }

      void subtract_subtotal(const area_weighted_patch_size& subtotal, const Weight& w)
      {
        m_sum -= w * subtotal.m_sum;
        m_weight -= w * subtotal.m_weight;
      }

      optional<double> extract() const
      {
        if (m_weight == 0) {
          return none;
        }
        else {
          return m_sum / m_weight;
        }
      }

      double m_sum;
      double m_weight;
    };

    struct area_weighted_patch_size_generator
    {
      using indicator = area_weighted_patch_size<>;

      indicator operator()() const
      {
        return indicator{};
      }
    };
  }
}
