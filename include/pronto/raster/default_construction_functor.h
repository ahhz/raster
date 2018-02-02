//
//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// This class is not currently in use. Simplification in the Pronto Raster 
// API have made it obsolete. 
//
// This class is used as a default for functions that allow specifying a 
// construction functor.
// The idea is that a class can contain a constructing object, without the need 
// of adding a template parameter to the class 
//

/* Example

template<class T> struct foo
{
  template<class Maker = default_construction_functor<T> >
  foo(Maker maker = Maker() ) : m_maker(maker) {}

  void bar() {
    T t = m_maker();
  }

private:
  construction_functor<T> m_maker;
};

*/
// 

#pragma once

#include <memory>
#include <type_traits>

namespace pronto {
  namespace raster {
      
    template<typename T>
    struct construction_functor_base
    {
      virtual T operator()() const = 0;
    };

    template<typename T, typename TMaker>
    struct construction_functor_helper : construction_functor_base < T >
    {
      construction_functor_helper(const TMaker& maker) : m_maker(maker)
      {
      }

      T operator()()const
      {
        return m_maker();
      }
    private:
      TMaker m_maker;
    };



    template<typename T>
    struct default_construction_functor : construction_functor_base < T >
    {
      T operator()() const 
      {
        return T();
      }
    };

    template<typename T>
    struct construction_functor
    {
      construction_functor() : m_maker(new default_construction_functor<T>)
      {
      }
      construction_functor(const construction_functor&) = default;
      construction_functor(construction_functor&&) = default;
      construction_functor& operator=(const construction_functor&) = default;
      construction_functor& operator=(construction_functor&&) = default;

      template<typename TMaker, typename = std::enable_if<
        !std::is_same<TMaker, construction_functor>::value> >
      construction_functor(TMaker&& maker)
        : m_maker(new construction_functor_helper<T, TMaker>
        (std::forward<TMaker>(maker)))
      {
      }

      template<typename TMaker>
      void reset(const TMaker& maker)
      {
        m_maker.reset(new construction_functor_helper<T, TMaker>(maker));
      }

      T operator()() const
      {
        return (*m_maker)();
      }

    private:
      std::shared_ptr<construction_functor_base<T> > m_maker;
    };
  }
} 

