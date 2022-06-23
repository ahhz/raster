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
    class raster_variant
    {
    public:
      static const bool is_raster_variant = true; // to facilitate concept check
      template<class T>
      raster_variant(type_erased_raster < T, IterationType, AccessType> raster)
      {
        m_raster = raster;
      }

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
        type_erased_raster< std::optional<double>, IterationType, AccessType > > m_raster;
    };

    template<class R>
    auto erase_and_hide_raster_type(R r)
    {
      return raster_variant{ erase_raster_type(r) };
    }
  }
}
