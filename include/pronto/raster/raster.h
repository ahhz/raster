//
//=======================================================================
// Copyright 2022
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
#pragma once

#include <ranges>

namespace pronto {
  namespace raster {

    template <class R>
    concept RasterConcept = std::ranges::sized_range<R> && std::ranges::view<R> &&
      requires(R & __r) {
      __r.rows();
      __r.cols();
      __r.sub_raster(int{}, int{}, int{}, int{});
    } ;

    //TODO these can be defined more elegantly
    template <class R>
    concept TypeErasedRasterConcept = RasterConcept<R> && requires {
      R::is_type_erased;
    } && R::is_type_erased;


    template <class R>
    concept RasterVariantConcept = R::is_raster_variant;

    template <class R> 
    concept NoRasterConcept = (!RasterConcept<R>) && (!RasterVariantConcept<R>);
  }
}