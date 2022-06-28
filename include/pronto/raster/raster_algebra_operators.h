//
//=======================================================================
// Copyright 2016-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// MACRO to provide operator overloads for range_algebra_wrappers
//
// Overload raster_algebra operator "op" and ties it to function "func".

#pragma once

#include <pronto/raster/optional.h>
#include <pronto/raster/raster.h>
#include <pronto/raster/raster_variant.h>

#include <functional>



namespace pronto {
  namespace raster {
    namespace detail {

      template<class T, class F>
      struct left_tie
      {
        left_tie(T a, F f) : m_a(a), m_f(f)
        {}
        template<class U>
        auto operator()(const U& b)
        {
          return m_f(m_a, b);
        }
        T m_a;
        F m_f;
      };

      template<class T, class F>
      struct right_tie
      {
        right_tie(T b, F f) : m_b(b), m_f(f)
        {}

        template<class U>
        auto operator()(const U& a)
        {
          return m_f(a, m_b);
        }
        T m_b;
        F m_f;
      };

      template<class F, RasterConcept RA, RasterConcept RB>
      auto binary_operator_apply(F f, RA a, RB b) {
        return transform(optionalize_function(f), a, b);
      }

      template<class F, RasterConcept RA, NoRasterConcept T>
      auto binary_operator_apply(F f, RA a, T vb) {
        return transform(optionalize_function(right_tie{ vb, f }), a);
      }

      template<class F, NoRasterConcept T, RasterConcept RB>
      auto binary_operator_apply(F f, T va, RB b) {
        return transform(optionalize_function(left_tie{ va, f }), b);
      }

      template<class F, RasterConcept RA, RasterConcept RB>
        requires TypeErasedRasterConcept<RA> || TypeErasedRasterConcept<RB>
      auto  binary_operator_apply(F f, RA a, RB b) {
        return erase_raster_type(transform(optionalize_function(f), a, b));
      }

      template<class F, TypeErasedRasterConcept RA, NoRasterConcept T>
      auto  binary_operator_apply(F f, RA a, T vb) {
        return erase_raster_type(transform(optionalize_function(right_tie{ vb, f }), a));
      }

      template<class F, NoRasterConcept T, TypeErasedRasterConcept RB>
      auto  binary_operator_apply(F f, T va, RB b) {
        return erase_raster_type(transform(optionalize_function(left_tie{ va, f }), b));
      }

      template<class F, RasterVariantConcept RA, RasterVariantConcept RB>
      auto  binary_operator_apply(F f, RA variant_a, RB variant_b) {
        return std::visit([=](auto&& a, auto&& b) {
          return erase_and_hide_raster_type(transform(optionalize_function(f), a, b)); }, variant_a, variant_b);
      }

      template<class F, RasterConcept RA, RasterVariantConcept RB>
      auto binary_operator_apply(F f, RA a, RB variant_b) {
        return std::visit([=](auto&& b) {
          return erase_and_hide_raster_type(transform(optionalize_function(f), a, b)); }, variant_b);
      }

      template<class F, RasterVariantConcept RA, RasterConcept RB>
      auto binary_operator_apply(F f, RA variant_a, RB b) {
        return std::visit([=](auto&& a, auto&& b) {
          return erase_and_hide_raster_type(transform(optionalize_function(f), a, b)); }, variant_a);
      }

      template<class F, NoRasterConcept T, RasterVariantConcept RB>
      auto  binary_operator_apply(F f, T va, RB variant_b) {
        return std::visit([=](auto&& b) {return erase_and_hide_raster_type(transform(optionalize_function(left_tie{ va, f }), b)); }, variant_b);
      }
      
      template<class F, RasterVariantConcept RA, NoRasterConcept T >
      auto  binary_operator_apply(F f, RA variant_a, T vb) {
         return std::visit([=](auto&& a) {return erase_and_hide_raster_type(transform(optionalize_function(right_tie{ vb, f }), a)); }, variant_a);
      }

      template<class F, RasterConcept R>
      auto unary_operator_apply(F f, R a) {
        return transform(optionalize_function(f), a);
      }
      template<class F, TypeErasedRasterConcept R>
      auto unary_operator_apply(F f, R a) {
        return erase_raster_type(transform(optionalize_function(f), a));
      }

      template<class F, RasterVariantConcept RA>
      auto unary_operator_apply(F f, RA variant_a) {
        return std::visit([=](auto&& a) {
          return erase_and_hide_raster_type(transform(optionalize_function(f), a)); }, variant_a);
      }
    }
  }
}

#define PRONTO_RASTER_MAP_ALGEBRA_BINARY_OP(op, func)       \
namespace pronto {                                          \
                                                            \
    namespace raster {                                      \
                                                            \
      template<class RA, class RB>                          \
      requires RasterConcept<RA> ||                         \
               RasterConcept<RB> ||                         \
               RasterVariantConcept<RA> ||                  \
               RasterVariantConcept<RB>                     \
      auto operator op(RA a, RB b) {                        \
                                                            \
        return detail::binary_operator_apply(func{}, a, b); \
    }                                                       \
  }                                                         \
}

#define PRONTO_RASTER_MAP_ALGEBRA_UNARY_OP(op, func)        \
namespace pronto {                                          \
                                                            \
    namespace raster {                                      \
                                                            \
      template<class R>                                     \
      requires RasterConcept<R> ||                          \
               RasterVariantConcept<R>                      \
      auto operator op(R a) {                               \
                                                            \
        return detail::unary_operator_apply(func{}, a);     \
    }                                                       \
  }                                                         \
}

PRONTO_RASTER_MAP_ALGEBRA_BINARY_OP(+, std::plus)
PRONTO_RASTER_MAP_ALGEBRA_BINARY_OP(-, std::minus)
PRONTO_RASTER_MAP_ALGEBRA_BINARY_OP(/ , std::divides)
PRONTO_RASTER_MAP_ALGEBRA_BINARY_OP(%, std::modulus)
PRONTO_RASTER_MAP_ALGEBRA_BINARY_OP(*, std::multiplies)
PRONTO_RASTER_MAP_ALGEBRA_BINARY_OP(&&, std::logical_and)
PRONTO_RASTER_MAP_ALGEBRA_BINARY_OP(|| , std::logical_or)
PRONTO_RASTER_MAP_ALGEBRA_BINARY_OP(> , std::greater)
PRONTO_RASTER_MAP_ALGEBRA_BINARY_OP(>= , std::greater_equal)
PRONTO_RASTER_MAP_ALGEBRA_BINARY_OP(< , std::less)
PRONTO_RASTER_MAP_ALGEBRA_BINARY_OP(<= , std::less_equal)
PRONTO_RASTER_MAP_ALGEBRA_BINARY_OP(== , std::equal_to)
PRONTO_RASTER_MAP_ALGEBRA_BINARY_OP(!= , std::not_equal_to)

PRONTO_RASTER_MAP_ALGEBRA_UNARY_OP(-, std::negate)
PRONTO_RASTER_MAP_ALGEBRA_UNARY_OP(!, std::logical_not)
