//
//=======================================================================
// Copyright 2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
/* Remove all contents of this file: superceded by type_erased_raster and raster_variant*/

/*
#pragma once

#include <pronto/raster/any_blind_raster.h>
#include <pronto/raster/any_raster.h>
#include <pronto/raster/assign.h>
#include <pronto/raster/io.h>
#include <pronto/raster/transform_raster_view.h>
#include <pronto/raster/uniform_raster_view.h>

namespace pronto {
  namespace raster {

    template<class Raster>
    class raster_algebra_wrapper
    {
      using sub_raster_type = raster_algebra_wrapper<typename traits<Raster>::sub_raster> ;
  
    public:
      using iterator = typename traits<Raster>::iterator;
      using const_iterator = typename traits<Raster>::const_iterator;

      raster_algebra_wrapper(Raster r) : m_raster(r)
      {}

      int rows() const
      {
        return m_raster.rows();
      }
      int cols() const
      {
        return m_raster.cols();
      }
      iterator begin()
      {
        return m_raster.begin();
      }

      iterator end()
      {
        return m_raster.end();
      }

      const_iterator begin() const
      {
        return m_raster.begin();
      }

      const_iterator end() const
      {
        return m_raster.end();
      }

      sub_raster_type sub_raster(int first_row, int first_col, int rows, int cols) const 
      {
        return sub_raster_type(m_raster.sub_raster(first_row, first_col, rows, cols));
      }

      Raster unwrap() const
      {
        return m_raster;
      }

    private:
      Raster m_raster;
    };
    
    template<>
    class raster_algebra_wrapper<any_blind_raster>
    {
    public:

      raster_algebra_wrapper(any_blind_raster r) : m_raster(r)
      {}

      int rows() const
      {
        return m_raster.rows();
      }
	  
      int cols() const
      {
        return m_raster.cols();
      }
   
      int size() const
      {
        return m_raster.size();
      }

      raster_algebra_wrapper<any_blind_raster> sub_raster(int first_row
        , int first_col, int rows, int cols)
      {
        auto sub = m_raster.sub_raster(first_row, first_col, rows, cols);
        return raster_algebra_wrapper<any_blind_raster>(sub);
      }

      any_blind_raster unwrap() const
      {
        return m_raster;
      }
 
    private:
      any_blind_raster m_raster;
    };

    template<class Raster>
    raster_algebra_wrapper<Raster> raster_algebra_wrap(Raster raster)
    {
      return raster_algebra_wrapper<Raster>(raster);
    }
  }
}
*/