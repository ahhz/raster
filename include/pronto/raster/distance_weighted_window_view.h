//
//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once

#include <pronto/raster/indicator_functions.h>
#include <pronto/raster/offset_raster_view.h>
#include <pronto/raster/optional_raster_view.h>
#include <pronto/raster/padded_raster_view.h>
#include <pronto/raster/raster_allocator.h>
#include <pronto/raster/subraster_window_view.h>
#include <pronto/raster/uniform_raster_view.h>
#include <cmath> // sqrt


namespace pronto {
  namespace raster {
    template<class IndicatorGenerator>
    class global_indicator
    {
    public:
      global_indicator(const IndicatorGenerator& gen) : m_generator(gen)
      {}

      using indicator_value_type = decltype(std::declval<IndicatorGenerator>()().extract());
    public:
      template<class Raster, class WeightRaster>
      indicator_value_type operator() (Raster raster, WeightRaster weight) const
      {
        auto indicator = m_generator();
        using value_type = typename traits<Raster>::value_type;
        using weight_type = typename traits<WeightRaster>::value_type;
        // todo make adder more robust for nodata / optional
        auto adder = [&](value_type v, weight_type w) {
          if (v) indicator.add_sample(*v, w); return 0; };
        
        auto t = transform(adder, raster, weight);
        for (auto&& x : t)
        {
        }
        return indicator.extract();
      }
    private:
      IndicatorGenerator m_generator;
    };

    template<class RasterAllocator = default_raster_allocator, class WeightType = double>
    struct weighted_window
    {

      template<class Function>
      weighted_window(double max_radius, const Function& f, RasterAllocator ra = RasterAllocator{})
      {
        m_kernel_radius = static_cast<int>(max_radius); // floor
        int kernel_size = 2 * m_kernel_radius + 1;
        m_kernel = ra.allocate<WeightType>(kernel_size, kernel_size);
        auto i = m_kernel.begin();
        for (int row = 0; row < kernel_size; ++row) {
          for (int col = 0; col < kernel_size; ++col, ++i) {
            int dx = (kernel_size / 2 - col); // dim/2 is rounded down
            int dy = (kernel_size / 2 - row); // dim/2 is rounded down
            double d = std::sqrt(dx * dx + dy * dy);
            *i = d <= max_radius ? f(d) : 0;// todo: be more sensible about nodata, e.g. kernel_mask
          }
        }
      }
      int m_kernel_radius;
      using kernel_type = decltype(std::declval<RasterAllocator>().allocate<double>(int{}, int{}));
      kernel_type m_kernel;
    };

    template<class Raster, class IndicatorGenerator, class RasterAllocator, class WeightType>
    struct distance_weighted_indicator_view_helper
    {
    private:
      using subraster_raster_type = subraster_window_view<optional_raster_view<Raster> >;
      using weight_raster_type = //typename RasterAllocator::type<WeightType>;
        decltype(std::declval<RasterAllocator>().allocate<double>(int{}, int{}));
      using weight_raster_raster_type = uniform_raster_view<weight_raster_type>;
      using transform_functor = global_indicator<IndicatorGenerator>;
      using raster_type = transform_raster_view<transform_functor, subraster_raster_type, weight_raster_raster_type>;

    public:
      using type = raster_type;
    };

    template<class Raster, class IndicatorGenerator, class RasterAllocator, class WeightType>
    using distance_weighted_indicator_view = typename distance_weighted_indicator_view_helper
      <Raster, IndicatorGenerator, RasterAllocator, WeightType>::type;


    template<class Raster, class IndicatorGenerator,
      class RasterAllocator = default_raster_allocator, class WeightType = double>
      distance_weighted_indicator_view<Raster, IndicatorGenerator, RasterAllocator, WeightType>
      make_distance_weighted_indicator_view(Raster raster, weighted_window<RasterAllocator,
        WeightType> window, IndicatorGenerator gen)
    {
      auto subraster_raster = make_square_subraster_window_view(optionalize(raster)
        , window.m_kernel_radius, std::nullopt);
      auto weight_raster_raster = uniform(raster.rows(), raster.cols(), window.m_kernel);
      auto global_indicator_function = global_indicator<IndicatorGenerator>(gen);
      return transform(global_indicator_function, subraster_raster, weight_raster_raster);
    }
  }
}
