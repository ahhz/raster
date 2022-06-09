//
//=======================================================================
// Copyright 2017-18
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once
#include <pronto/raster/any_raster.h>

#include <any>
#include <functional>

namespace pronto {
  namespace raster {

    
    class any_blind_raster
    {
    public:
      any_blind_raster() = default;

      template<typename T>
      any_blind_raster(any_raster<T> data)
        : m_data(data)
      {
        m_get_rows = [&]() {return get<T>().rows(); };
        m_get_cols = [&]() {return get<T>().cols(); };
        m_get_size = [&]() {return get<T>().size(); };
        m_get_sub_raster = [&](int a, int b, int c, int d) {
          return make_any_raster(get<T>().sub_raster(a, b, c, d));
        };
      }

      template<class T>
      any_raster<T> get() const // throws bad_any_cast for the wrong T
      {
        return std::any_cast<any_raster<T>>(m_data);
      }

      int rows() const
      {
        return m_get_rows();
      }

      int cols() const
      {
        return m_get_cols();
      }

      int size() const
      {
        return m_get_size();
      }

      any_blind_raster sub_raster(int r, int c, int rs, int cs) const
      {
        return m_get_sub_raster(r, c, rs, cs);
      }

    private:
      std::any m_data;
      std::function<int()> m_get_rows;
      std::function<int()> m_get_cols;
      std::function<int()> m_get_size;
      std::function<any_blind_raster(int, int, int, int)> m_get_sub_raster;
    };

    template<class Raster>
    any_blind_raster make_any_blind_raster(Raster r)
    {
      return any_blind_raster(make_any_raster(r));
    }
  }
}