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

#include <blink/raster/any_blind_raster.h>
#include <blink/raster/any_raster.h>
#include <blink/raster/assign.h>
#include <blink/raster/io.h>
#include <blink/raster/transform_raster_view.h>
#include <blink/raster/uniform_raster_view.h>

namespace blink {
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

    template<class F, class A, class B>
    struct applicator
    {
      using type = transform_raster_view<F, A, B>;

      static type f(F function, A a, B b)
      {
        return transform(function, a, b);
      }
    };

    template<class F, class A, class B>
    using return_type
      = decltype(std::declval<F>()(std::declval<A>(), std::declval<B>()));


    template<class F, class A, class U>
    struct applicator<F, A, any_raster<U> >
    {
      using T = typename traits<A>::value_type;
      using type = any_raster< return_type<F, T, U> >;

      static type f(F function, A a, any_raster<U> b)
      {
        return make_any_raster(transform(function, a, b));
      }
    };

    template<class F, class T, class B>
    struct applicator<F, any_raster<T>, B >
    {
      using U = typename traits<B>::value_type;
      using type = any_raster< return_type<F, T, U> >;

      static type f(F function, any_raster<T> a, B b)
      {
        return make_any_raster(transform(function, a, b));
      }
    };

    template<class F, class T, class U>
    struct applicator<F, any_raster<T>, any_raster<U> >
    {
      using type = any_raster< return_type<F, T, U> >;

      static type f(F function, any_raster<T> a, any_raster<U> b)
      {
        return make_any_raster(transform(function, a, b));
      }
    };

    template<class F, class A>
    struct applicator<F, A, any_blind_raster >
    {
      static any_blind_raster f(F function, A a, any_blind_raster b)
      {
        return blind_function(apply_second<F, A>(function, a), b);
      }
    };

    template<class F, class B>
    struct applicator<F, any_blind_raster, B >
    {
      static any_blind_raster f(F function, any_blind_raster a, B b)
      {
        return blind_function(apply_first<F, B>(function, b), a);

      }
    };

    template<class F>
    struct applicator<F, any_blind_raster, any_blind_raster>
    {
      static any_blind_raster f(F function, any_blind_raster a, any_blind_raster b)
      {
        return blind_function(apply_first_and_second<F>(function, a), b);
      }
    };

    template<class F, class T, class U>
    struct raster_algebra_apply;
    //{
    //  static_assert(false, "T or U must be a raster_algebra_wrapper");
    //};

    template<class F, class T, class U>
    struct raster_algebra_apply<F, raster_algebra_wrapper<T>, U>
    {
      using r1 = T;
      using r2 = uniform_raster_view<U>;
      using fun = applicator< F, r1, r2 >;
      using type = decltype(fun::f(std::declval<F>(),
        std::declval<r1>(), std::declval<r2>()));
      using wrapped = raster_algebra_wrapper<type>;

      static wrapped f(F function, raster_algebra_wrapper<T> t, U u)
      {
        return raster_algebra_wrap(
          fun::f(function, t.unwrap(), uniform(t.rows(), t.cols(), u)));
      };
    };

    template<class F, class T, class U>
    struct raster_algebra_apply<F, T, raster_algebra_wrapper<U> >
    {
      using r1 = uniform_raster_view<T>;
      using r2 = U;
      using fun = applicator< F, r1, r2 >;
      using type = decltype(fun::f(std::declval<F>(),
        std::declval<r1>(), std::declval<r2>()));
      using wrapped = raster_algebra_wrapper<type>;

      static wrapped f(F function, T t, raster_algebra_wrapper<U> u)
      {
        return raster_algebra_wrap(
          fun::f(function, uniform(u.rows(), u.cols(), t), u.unwrap()));
      };
    };

    template<class F, class T, class U>
    struct raster_algebra_apply
      <F, raster_algebra_wrapper<T>, raster_algebra_wrapper<U> >
    {
      using r1 = T;
      using r2 = U;
      using fun = applicator< F, r1, r2 >;
      using type = decltype(fun::f(std::declval<F>(),
        std::declval<r1>(), std::declval<r2>()));
      using wrapped = raster_algebra_wrapper<type>;

      static wrapped f(F function, raster_algebra_wrapper<T> t
        , raster_algebra_wrapper<U> u)
      {
        return raster_algebra_wrap(fun::f(function, t.unwrap(), u.unwrap()));
      };
    };

    // Binary operators
    // Requires T or U is raster_algebra_wrapper
    template<class F, class T, class U>
    auto raster_algebra(F function, T t, U u)
      -> decltype(raster_algebra_apply<F, T, U>::f(function, t, u))
    {
      return raster_algebra_apply<F, T, U>::f(function, t, u);
    }
    
    // Unary operators
    template<class F, class T>
    auto raster_algebra(F function, raster_algebra_wrapper<T> t)
      -> decltype(raster_algebra_wrap(transform_raster(function, t.unwrap())))
    {
      return raster_algebra_wrap(transform_raster(function, t.unwrap()));
    }

    template<class F>
    auto raster_algebra(F function, raster_algebra_wrapper<any_blind_raster> t)
      -> decltype(raster_algebra_wrap(blind_operator(function, t.unwrap())))
    {
      return raster_algebra_wrap(blind_function(apply_unary<F>(function), t.unwrap()););
    }

    struct create_and_assign_helper
    {
      create_and_assign_helper(const filesystem::path& path) : m_path(path)
      {}

      template<class T>
      any_blind_raster operator()(const any_raster<T>& in)
      {
        using in_value_type = T;
        using out_value_type = typename gdal_class<in_value_type>::type;
        auto out = create<out_value_type>(m_path, in.rows(), in.cols());
        assign_blocked(out, in, 256, 256);
        return make_any_blind_raster(out);
      }

    private:
      filesystem::path m_path;

    };
    template<class Raster>
    struct create_and_assign_helper_2
    {
      any_blind_raster operator()(const Raster& in, const filesystem::path& path)
      {
        using in_value_type = typename traits<Raster>::value_type;
        using out_value_type = typename gdal_class<in_value_type>::type;
        auto out = create<out_value_type>(path, in.rows(), in.cols());
        assign_blocked(out, in, 256, 256);
        return make_any_blind_raster(out);
      }
    };

    template<class Raster>
    struct create_and_assign_helper_2<raster_algebra_wrapper<Raster> >
    {
      any_blind_raster operator()(const raster_algebra_wrapper<Raster>& in, const filesystem::path& path)
      {
        using in_value_type = typename traits<Raster>::value_type;
        using out_value_type = typename gdal_class<in_value_type>::type;
        auto out = create<out_value_type>(path, in.rows(), in.cols());
        assign_blocked(out, in.unwrap(),256,256);
        return make_any_blind_raster(out);
      }
    };

    template<>
    struct create_and_assign_helper_2<any_blind_raster>
    {
      any_blind_raster operator()(const any_blind_raster& in, const filesystem::path& path)
      {
        return blind_function(create_and_assign_helper(path), in);
      }
    };

    template<>
    struct create_and_assign_helper_2<raster_algebra_wrapper<any_blind_raster> >
    {
      any_blind_raster operator()(const raster_algebra_wrapper<any_blind_raster>& in, const filesystem::path& path)
      {
        return blind_function(create_and_assign_helper(path), in.unwrap());
      }
    };

    template<class Raster>
    any_blind_raster create_and_assign(const Raster& in, const filesystem::path& path)
    {
      return create_and_assign_helper_2<Raster>{}(in, path);
    }
  }
}
