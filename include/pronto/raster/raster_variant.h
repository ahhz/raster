//
//=======================================================================
// Copyright 2022
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once

#include <pronto/raster/access_type.h>
#include <pronto/raster/type_erased_raster.h>

#include <optional>
#include <variant>

namespace pronto {
  namespace raster {

    template<iteration_type IterationType = iteration_type::multi_pass, access AccessType = access::read_write>
    using raster_variant = 
      std::variant<
        type_erased_raster< bool, IterationType, AccessType >,
        type_erased_raster< uint8_t, IterationType, AccessType >,
        type_erased_raster< int16_t, IterationType, AccessType >,
        type_erased_raster< uint16_t, IterationType, AccessType >,
        type_erased_raster< int32_t, IterationType, AccessType >,
        type_erased_raster< uint32_t, IterationType, AccessType >,
        type_erased_raster< float, IterationType, AccessType >,
        type_erased_raster< double, IterationType, AccessType >,
        type_erased_raster< std::optional<bool>, IterationType, AccessType >,
        type_erased_raster< std::optional<uint8_t >, IterationType, AccessType >,
        type_erased_raster< std::optional<int16_t >, IterationType, AccessType >,
        type_erased_raster< std::optional<uint16_t >, IterationType, AccessType >,
        type_erased_raster< std::optional<int32_t >, IterationType, AccessType >,
        type_erased_raster< std::optional<uint32_t>, IterationType, AccessType >,
        type_erased_raster< std::optional<float>, IterationType, AccessType >,
        type_erased_raster< std::optional<double>, IterationType, AccessType > > ;
 
    template<class R>
    auto erase_and_hide_raster_type(R r)
    {
      constexpr auto iter_type = std::ranges::forward_range<R> ? iteration_type::multi_pass : iteration_type::single_pass;
      constexpr auto writeable = std::ranges::output_range < R, std::ranges::range_value_t<R> >;
      constexpr auto ref_equals_value = std::is_same_v< std::ranges::range_reference_t<R>, std::ranges::range_value_t<R>>;
      constexpr auto acc_type = writeable && !ref_equals_value ? access::read_write : access::read_only;
      
      return raster_variant<iter_type, acc_type>{ erase_raster_type(r) };
    }
  }
}
