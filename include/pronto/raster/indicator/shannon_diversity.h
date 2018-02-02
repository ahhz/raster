
//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================

#pragma once

#include <pronto/raster/optional.h>

#include <numeric>
#include <utility>
#include <vector>

namespace pronto {
  namespace raster {

    struct counted_vector
    {
      void increment(int index, double w)
      {
        for (int i = static_cast<int>(counts.size()); i <= index; ++i)  {
          counts.push_back(0);
        }
        counts[index] += w;
      }

      void decrement(int index, double w)
      {
        counts[index] -= w;
      }

      std::vector<double> counts;
    };
    struct shannon_diversity
    {
      using value_type = int;
      using weight_type = double;

      void add_sample(const value_type& v, const weight_type& w)
      {
        cv.increment(v, w);
      }

      void add_sample(const value_type& v)
      {
        cv.increment(v, 1.0);
      }

      void subtract_sample(const value_type& v, const weight_type& w)
      {
         cv.decrement(v, w);
      }

      void subtract_sample(const value_type& v)
      {
        cv.decrement(v, 1.0);
      }

      void add_subtotal(const shannon_diversity& subtotal, const weight_type& w)
      {
        const int n = static_cast<int>(subtotal.cv.counts.size());
        for (int i = 0; i < n; ++i) {
          add_sample(i, subtotal.cv.counts[i] * w);
        }
      }

      void add_subtotal(const shannon_diversity& subtotal)
      {
        const int n = static_cast<int>(subtotal.cv.counts.size());
        for (int i = 0; i < n; ++i) {
          add_sample(i, subtotal.cv.counts[i]);
        }
      }

      void subtract_subtotal(const shannon_diversity& subtotal, const weight_type& w)
      {
        const int n = static_cast<int>(subtotal.cv.counts.size());
        for (int i = 0; i < n; ++i) {
          subtract_sample(i, subtotal.cv.counts[i] * w);
        }
      }

      void subtract_subtotal(const shannon_diversity& subtotal)
      {
        const int n = static_cast<int>(subtotal.cv.counts.size());
        for (int i = 0; i < n; ++i) {
          subtract_sample(i, subtotal.cv.counts[i]);
        }
      }

      double extract() const
      {
        double sum = 0;
        sum = std::accumulate(cv.counts.begin(), cv.counts.end(), sum);
        double h = 0;
        for (auto i : cv.counts) {
          if (i > 0) {
            const double pi = i / sum;
            h -= pi * std::log(pi);
          }
        }
        return h;
      }
      counted_vector cv;
    };

    struct shannon_diversity_generator
    {
      using indicator = shannon_diversity;
      indicator operator()() const
      {
        return indicator{};
      }
    };
  }
}
