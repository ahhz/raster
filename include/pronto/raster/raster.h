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
#include <variant>
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


    template<class R>
    constexpr auto is_raster_variant_v = false;

    template<RasterConcept... R>
    constexpr auto is_raster_variant_v<std::variant<R...> > = true;

    template <class R>
    concept RasterVariantConcept = is_raster_variant_v<R>;

    template <class R> 
    concept NoRasterConcept = (!RasterConcept<R>) && (!RasterVariantConcept<R>);


    template<class F, class... Args >
    auto visit_with_variant_output(F f, std::variant<Args...>&& var)
    {
      using output_type = std::variant<decltype(std::declval<F>()(std::declval<Args>()))...>;
      return std::visit([](auto&& v) {return output_type{ f(v) }; }, std::forward< std::variant<Args...> >(var));
    }
  }
}