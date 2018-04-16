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

#include <pronto/raster/any.h>
#include <pronto/raster/any_raster.h>

#include <functional>
#include <vector>
 
#pragma warning( push )  
#pragma warning( disable : 4244 )  // Suppressing warning related to casting,
#pragma warning( disable : 4267 )  // these are inherent to any_blind_raster

namespace pronto {
  namespace raster {

    using blind_data_types = std::tuple
      < bool
      , uint8_t
      , int16_t
      , uint16_t
      , int32_t
      , uint32_t
      , float
      , double>;
    
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
      any_raster<T> get()  // will throw if you specify the wrong T
      {
        return any_cast<any_raster<T> >(m_data);
      }
	
	private:
	  template<class T> bool check()
	  {
		  try
		  {
			  get<T>();
		  }
		  catch (const bad_any_cast&)
		  {
			  return false;
		  }
		  return true;
	  }
	
	  template<std::size_t I> int index_in_list_helper()
	  {
		  return -1;
	  }
	  
	  template<std::size_t I, class T, class... Rest>
	  int index_in_list_helper()
	  {
		  if (check<T>()) return I;
		  return index_in_list_helper<I + 1, Rest...>();
	  }
	  
	  template<class... T>
	  int index_in_list()
	  {
		  return index_in_list_helper<0, T...>();
	  }

	  template< template<class...> class Pack, class...T>
	  int index_in_packed_list(const Pack<T...>&)
	  {
		  return index_in_list<T...>();
	  }

	public:
	  int index()
	  {
		  return index_in_packed_list(blind_data_types{}); 
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
        return m_get_sub_raster(r,c,rs,cs);
      }

    private:
      any m_data; 

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
#pragma warning( pop )