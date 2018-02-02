//
//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// This is not currently in use:  Pronto Raster does not support complex
// numbers. When we start supporting it, we will probably use STL classes
// instead.

#pragma once

namespace pronto
{
  namespace raster
  {
    template<class T>
    struct complex
    {
      complex() = default;
      complex(const complex&) = default;
      complex(complex&&) = default;
      complex(const T& r) : real(r), imaginary(T()) // To allow conversion
      {}
      complex(const T& r, const T& i) : real(r), imaginary(i) // To be consistent
      {}

      template<class U >
      complex(const complex<U>& that) 
        : real(static_cast<T>(that.real))
        , imaginary(static_cast<T>(that.imaginary))
      {}
      complex& operator=(const complex&) = default;
      complex& operator=(complex&&) = default;

      operator T() const // To allow conversion
      {
        return real;
      }

      T real;
      T imaginary;
    };
    using cint16_t = complex<int16_t>;
    using cint32_t = complex<int32_t>;
    using cfloat32_t = complex<float>;
    using cfloat64_t = complex<double>;
  }
}