//
//=======================================================================
// Copyright 2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once

namespace blink {
  namespace raster {
      
    // As of C++14 this is part of the std library:
    //   std::make_index_sequence 
    //   template<std::size_t...> struct std::index_sequence{};
    
    template<std::size_t ...>
    struct index_sequence { };

    namespace detail {
      template<std::size_t N, std::size_t ...S>
      struct make_index_sequence_helper : make_index_sequence_helper < N - 1, N - 1, S... >
      { };

      template<std::size_t ...S>
      struct make_index_sequence_helper < 0, S... >
      {
        using type = index_sequence < S... >;
      };
    }

    template<std::size_t N>
    using make_index_sequence = typename detail::make_index_sequence_helper<N>::type;
  }
}

