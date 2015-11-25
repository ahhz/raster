//
//=======================================================================
// Copyright 2015
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// This header file provides the default raster view for iterators that are 
// initialized by the raster and implement the find_begin and find_end functions


#ifndef BLINK_RASTER_EDGE_VIEW_H_AHZ
#define BLINK_RASTER_EDGE_VIEW_H_AHZ

#include <blink/raster/default_raster_view.h>
#include <blink/raster/edge_iterator.h>
#include <blink/raster/raster_iterator.h>
#include <blink/raster/raster_view.h>

namespace blink {
  namespace raster {

    template<class Raster, class GetStrategy>
    class h_edge_view : public default_raster_view<Raster,
      h_edge_iterator, GetStrategy>
    {
    public: 
      h_edge_view(Raster* r) : default_raster_view(r)
      {}

      index_type size1() const
      {
        return m_raster->size1() + 1;

      }

      index_type size2() const
      {
        return m_raster->size2();

      }
    };

    template<class Raster, class GetStrategy>
    class v_edge_view : public default_raster_view<Raster,
      v_edge_iterator, GetStrategy >
    {
    public:
      v_edge_view(Raster* r) : default_raster_view(r)
      {}

      index_type size1() const
      {
        return m_raster->size1();
      }

      index_type size2() const
      {
        return m_raster->size2() + 1;
      }
    };

    template<class Raster, class GetStrategy>
    class h_edge_trans_view : public default_raster_view<Raster,
      h_edge_trans_iterator, GetStrategy>
    {
    public:
      h_edge_trans_view(Raster* r) : default_raster_view(r)
      {}

      index_type size1() const
      {
        return m_raster->size2();

      }

      index_type size2() const
      {
        return m_raster->size1() + 1;

      }
    };

    template<class Raster, class GetStrategy>
    class v_edge_trans_view : public default_raster_view<Raster,
      v_edge_trans_iterator, GetStrategy>
    {
    public:
      v_edge_trans_view(Raster* r) : default_raster_view(r)
      {}

      index_type size1() const
      {
        return m_raster->size2() + 1;

      }

      index_type size2() const
      {
        return m_raster->size1();

      }
    };


    namespace detail {

      namespace gs = get_strategy;
      namespace el = element;

      template<class Element>
      struct strategy_lookup_helper
      {
        struct must_be_specified{};
        using type = must_be_specified;
      };
   
      template<> 
      struct strategy_lookup_helper<el::h_edge>
      {
        using type = gs::both;
      };
      
      template<> 
      struct strategy_lookup_helper<el::v_edge>
      {
        using type = gs::both;
      };
      
      template<> 
      struct strategy_lookup_helper<el::h_edge_first_only>
      {
        using type = gs::first_only;
      };
      
      template<> 
      struct strategy_lookup_helper<el::v_edge_first_only>
      {
        using type = gs::first_only;
      };
      template<> 
      struct strategy_lookup_helper<el::h_edge_second_only>
      {
        using type = gs::second_only;
      };

      template<> 
      struct strategy_lookup_helper<el::v_edge_second_only>
      {
        using type = gs::second_only;
      };

      template<class Raster, class Access>
      struct raster_lookup_helper
      {
        struct must_be_specified{};
        using type = must_be_specified;
      };

      template<class Raster>
      struct raster_lookup_helper<Raster, access::read_only>
      {
        using type = typename const Raster;
      };

      template<class Raster>
      struct raster_lookup_helper<Raster, access::read_write>
      {
        using type = typename Raster;
      };


           
      //gs::both strategy_lookup_function(el::h_edge);
      //gs::first_only strategy_lookup_function(el::h_edge_first_only);
      //gs::second_only strategy_lookup_function(el::h_edge_second_only);
      //gs::both strategy_lookup_function(el::v_edge);
      //gs::first_only strategy_lookup_function(el::v_edge_first_only);
      //gs::second_only strategy_lookup_function(el::v_edge_second_only);
      //template<class Element>
      //using strategy_lookup
      //  = decltype(strategy_lookup_function(std::declval<Element>()));
     
      template<class Element>
      using strategy_lookup =  typename strategy_lookup_helper<Element>::type;

      //template<class Raster>
      //const Raster raster_lookup_function(access::read_only);

     // template<class Raster>
      //typename Raster raster_lookup_function(access::read_write);

     // template<class Access, class Raster>
      //using raster_lookup
     //   = decltype(raster_lookup_function<Raster>(std::declval<Access>()));
      
      template<class Access, class Raster>
      using raster_lookup = typename raster_lookup_helper<Raster, Access>::type;

      template<class Orientation, class Element, class Access, class Raster>
      struct edge_view_lookup
      {
        struct needs_to_be_specialized{};
        using type = needs_to_be_specialized;
      };

      template <typename Access, class Raster>
      struct edge_view_lookup< orientation::row_major, element::h_edge,
        Access, Raster>
      {
        using type = h_edge_view<
          raster_lookup<Access, Raster>,
          strategy_lookup<element::h_edge> >;
      };

      template <typename Access, class Raster>
      struct edge_view_lookup< orientation::row_major, element::h_edge_first_only,
        Access, Raster>
      {
        using type = h_edge_view<
          raster_lookup<Access, Raster>,
          strategy_lookup<element::h_edge_first_only> >;
      };

      template <typename Access, class Raster>
      struct edge_view_lookup< orientation::row_major, element::h_edge_second_only,
        Access, Raster>
      {
        using type = h_edge_view<
          raster_lookup<Access, Raster>,
          strategy_lookup<element::h_edge_second_only> >;
      };

      template <typename Access, class Raster>
      struct edge_view_lookup< orientation::row_major, element::v_edge,
        Access, Raster>
      {
        using type = v_edge_view<
          raster_lookup<Access, Raster>,
          strategy_lookup<element::v_edge> >;
      };

      template <typename Access, class Raster>
      struct edge_view_lookup< orientation::row_major, element::v_edge_first_only,
        Access, Raster>
      {
        using type = v_edge_view<
          raster_lookup<Access, Raster>,
          strategy_lookup<element::v_edge_first_only> >;
      };

      template <typename Access, class Raster>
      struct edge_view_lookup< orientation::row_major, element::v_edge_second_only,
        Access, Raster>
      {
        using type = v_edge_view<
          raster_lookup<Access, Raster>,
          strategy_lookup<element::v_edge_second_only> >;
      };

      template <typename Access, class Raster>
      struct edge_view_lookup< orientation::col_major, element::h_edge,
        Access, Raster>
      {
        using type = h_edge_trans_view<
          raster_lookup<Access, Raster>,
          strategy_lookup<element::h_edge> >;
      };

      template <typename Access, class Raster>
      struct edge_view_lookup< orientation::col_major, element::h_edge_first_only,
        Access, Raster>
      {
        using type = h_edge_trans_view<
          raster_lookup<Access, Raster>,
          strategy_lookup<element::h_edge_first_only> >;
      };

      template <typename Access, class Raster>
      struct edge_view_lookup< orientation::col_major, element::h_edge_second_only,
        Access, Raster>
      {
        using type = h_edge_trans_view<
          raster_lookup<Access, Raster>,
          strategy_lookup<element::h_edge_second_only> >;
      };

      template <typename Access, class Raster>
      struct edge_view_lookup< orientation::col_major, element::v_edge,
        Access, Raster>
      {
        using type = v_edge_trans_view<
          raster_lookup<Access, Raster>,
          strategy_lookup<element::v_edge> >;
      };

      template <typename Access, class Raster>
      struct edge_view_lookup< orientation::col_major, element::v_edge_first_only,
        Access, Raster>
      {
        using type = v_edge_trans_view<
          raster_lookup<Access, Raster>,
          strategy_lookup<element::v_edge_first_only> >;
      };

      template <typename Access, class Raster>
      struct edge_view_lookup< orientation::col_major, element::v_edge_second_only,
        Access, Raster>
      {
        using type = v_edge_trans_view<
          raster_lookup<Access, Raster>,
          strategy_lookup<element::v_edge_second_only> >;
      };

    } //detail


    template<class Orientation, class Access, class Raster>
    struct raster_view_lookup<Orientation, element::h_edge, Access, Raster>
    {
      using type = typename detail::edge_view_lookup<Orientation, element::h_edge,
        Access, Raster>::type;
    };

    template<class Orientation, class Access, class Raster>
    struct raster_view_lookup<Orientation, element::h_edge_first_only, Access, Raster>
    {
      using type = typename detail::edge_view_lookup<Orientation, element::h_edge_first_only,
        Access, Raster>::type;
    };

    template<class Orientation, class Access, class Raster>
    struct raster_view_lookup<Orientation, element::h_edge_second_only, Access, Raster>
    {
      using type = typename detail::edge_view_lookup<Orientation, element::h_edge_second_only,
        Access, Raster>::type;
    };

    template<class Orientation, class Access, class Raster>
    struct raster_view_lookup<Orientation, element::v_edge, Access, Raster>
    {
      using type = typename detail::edge_view_lookup<Orientation, element::v_edge,
        Access, Raster>::type;
    };

    template<class Orientation, class Access, class Raster>
    struct raster_view_lookup<Orientation, element::v_edge_first_only, Access, Raster>
    {
      using type = typename detail::edge_view_lookup<Orientation, element::v_edge_first_only,
        Access, Raster>::type;
    };

    template<class Orientation, class Access, class Raster>
    struct raster_view_lookup<Orientation, element::v_edge_second_only, Access, Raster>
    {
      using type = typename detail::edge_view_lookup<Orientation, element::v_edge_second_only,
        Access, Raster>::type;
    };
  }
}
#endif