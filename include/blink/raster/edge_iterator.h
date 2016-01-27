//
//=======================================================================
// Copyright 2015
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// This file presents iterators over the edges between cells in a raster
//
// The iterators (try to) use the concepts as proposed in the context of the BOOST library
//
// http://www.boost.org/doc/libs/1_52_0/libs/iterator/doc/new-iter-concepts.html
//
// There are four edge-iterators
//
// h_edge_iterator 
//   - iterates over all horizontal edges in a raster
//   - line-by-line left-to-right.
//
// h_edge_trans_iterator 
//   - iterates over all horizontal edges in a raster
//   - column-by-column top-to-bottom.
//
//  v_edge_iterator 
//   - iterates over all vertical edges in a raster
//   - line-by-line left-to-right.
//
// v_edge_trans_iterator 
//   - iterates over all vertical edges in a raster
//   - column-by-column top-to-bottom.
//
// Dereferencing an edge_iterator gives two boost::optional-wrapped cell values
// h_edge iterators consist of one above the edge (first) and one below the edge (second).
// v_edge_iterators consist of one left of the edge (first) and one right of the edge (second).
//
// When there is no cell to the left/right/top/bottom of an edge, then the associated value 
// of the optional cell iterator is boost::none.
//
// The coordinate of a h_edge is that of the cell below.
// The coordinate of a v_edge is that of the cell to the right.
//
// This is chosen to the effect that coordinates are always positive: 0 <= i <= size1 and 0 <= j <= size2
//
// The following member functions are provided:
//   constructor(raster)
//   std::pair<Value, Value> get() gets the values next to the edge
//   operator++() increment iterator
//   find_begin(), find_end()
//   find(coordinates)
//   coordinates get_coordinates()
//
// there are three different get-strategies
// both      : return both values along the edge, if present.
// left_only : replaces right for boost::none
// right_only: replaces left for boost::none
// TODO: There is some code duplication here.
//

#ifndef BLINK_RASTER_EDGE_ITERATOR_H_AHZ
#define BLINK_RASTER_EDGE_ITERATOR_H_AHZ

#include <blink/raster/raster_iterator.h>
#include <blink/raster/raster_traits.h>
#include <blink/raster/raster_view.h>
#include <blink/raster/exceptions.h>
#include <blink/raster/dereference_proxy.h>

#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/optional/optional.hpp>

#include <type_traits>
#include <utility>

namespace blink {
  namespace raster {

    namespace get_strategy
    {
      struct both{};
      struct first_only{};
      struct second_only{};
    };

    template<class Raster, class GetStrategy> class edge_iterator_common; // forward

    namespace detail {

      template<class EdgeIterator, class Raster, class GetStrategy>
      struct edge_iterator_helper
      {
        using raster_value = raster_traits::value_type<typename Raster>;
        using opt_raster_value = boost::optional<raster_value>;
        using value = std::pair <opt_raster_value, opt_raster_value>;
        using access = boost::random_access_traversal_tag;
        using difference = std::ptrdiff_t;

        //
        using iterator = typename EdgeIterator;
        using reference = dereference_proxy<iterator, value>;
        using facade = boost::iterator_facade<iterator, value, access,
          reference, difference>;

        // using trans_iterator = gdal_iterator<Raster>;
        // using trans_reference = dereference_proxy<iterator, value>;
        // using trans_facade = boost::iterator_facade<trans_iterator, value,
        //   access, trans_reference, difference>;
      };
      template< typename EdgeIterator, typename Raster, typename GetStrategy>
      class edge_iterator_common : public edge_iterator_helper<EdgeIterator, Raster,
        GetStrategy>::facade
      {
        using this_type = edge_iterator_common<EdgeIterator, Raster, GetStrategy>;
      public:
        using coordinate_type = typename Raster::coordinate_type;
        using index_type = typename coordinate_type::index_type;
        using optional_pixel_value =
          boost::optional<typename Raster::value_type>;
        using value_type = std::pair<optional_pixel_value, 
          optional_pixel_value>;
        using reference = dereference_proxy<typename EdgeIterator, value_type>;
 
        // necessary for iterator_facade
        friend class boost::iterator_core_access;

        reference dereference() const
        {
          return reference(dynamic_cast<const EdgeIterator*>(this) );
        }

        // necessary for iterator_facade
        template<class OtherIterator>
        bool equal(const OtherIterator& other) const
        {
          return get_coordinates() == other.get_coordinates();
        }

        // necessary for iterator_facade
        void advance(std::ptrdiff_t n)
        {
          index_type i = index() + n;
          coordinate_type c = index_to_coordinate(i);
          find(c);
        }

        template<class OtherIterator>
        std::ptrdiff_t distance_to(const OtherIterator& other) const
        {
          return other.index() - index();
        }

        virtual coordinate_type get_coordinates() const = 0;

        value_type get() const
        {
          return get_specialized(GetStrategy());
        }

        value_type get_specialized(get_strategy::both) const
        {
          optional_pixel_value v1 = get_1();
          optional_pixel_value v2 = get_2();
          return std::make_pair(v1, v2);
        }

        value_type get_specialized(get_strategy::first_only) const
        {
          optional_pixel_value v1 = get_1();
          optional_pixel_value v2 = boost::none;
          return std::make_pair(v1, v2);
        }

        value_type get_specialized(get_strategy::second_only) const
        {
          optional_pixel_value v1 = boost::none;
          optional_pixel_value v2 = get_2();
          return std::make_pair(v1, v2);
        }


         value_type put(const value_type& value)
        {
          put_iterator_1(value.first);
          put_iterator_2(value.second);
        }

        template<class OptionalIterator, class OptionalValue>
        void put_iterator(OptionalIterator& i, OptionalValue& v)
        {

          if (i) {
            if (!v) {
              BOOST_THROW_EXCEPTION(assigning_an_uninitialized_optional_to_an_initialized_iterator{});
            }
            else {
              **i = *v;
            }
          }
          if (!i){
            if (v) {
              BOOST_THROW_EXCEPTION(assigning_an_initialized_optional_to_an_uninitialized_iterator{});
            }
          }
        }

        virtual void find(const coordinate_type& coordinates) = 0;
        virtual optional_pixel_value get_1() const = 0;
        virtual optional_pixel_value get_2() const = 0;
        virtual index_type index() const = 0;
        virtual coordinate_type index_to_coordinate(index_type i) const = 0;

      };
    } //detail

    template<typename Raster, typename GetStrategy = get_strategy::both>
    class h_edge_iterator : public detail::edge_iterator_common<
      h_edge_iterator<Raster, GetStrategy>, Raster, GetStrategy>
    {
      using pixel_view = raster_view<orientation::row_major, element::pixel,
        access::read_write, Raster>;

      using pixel_iterator = typename pixel_view::iterator;
  
    public:
      h_edge_iterator(Raster* r = NULL) : m_pixel_view(r)
      {}

      void find_begin()
      {
        m_iterator_first = boost::none;
        m_iterator_second = m_pixel_view.begin();
      }

      void find_end()
      {
        m_iterator_first = m_pixel_view.end();// TODO : or boost::none???
        m_iterator_second = boost::none;
      }

      // finds edge above given coordinates
      void find(const coordinate_type& coordinates)
      {
        if (coordinates.row > 0)	{
          if (!m_iterator_first) {
            m_iterator_first = m_pixel_view.begin();
          }
          m_iterator_first->find(coordinates + coordinate_type(-1, 0));
        }
        else {
          m_iterator_first = boost::none;
        }

        if (coordinates.row < size1())	{
          if (!m_iterator_second) {
            m_iterator_second = m_pixel_view.begin();
          }
          m_iterator_second->find(coordinates);
        }
        else {
          m_iterator_second = boost::none;
        }
      }

      // returns coordinates below current edge
      coordinate_type get_coordinates() const
      {
        if (m_iterator_second) return m_iterator_second->get_coordinates();
        return m_iterator_first->get_coordinates() + coordinate_type(1, 0);
      }

      // necessary for iterator_facade
      //friend class boost::iterator_core_access;

      // necessary for iterator_facade
      void increment()
      {
        if (m_iterator_first) {
          ++(*m_iterator_first);
        }
        else if (m_iterator_second->get_coordinates().col == size2() - 1) { 
          // the second iterator is about to progress from row 0 to row 1
          m_iterator_first = m_pixel_view.begin();
        }

        // Once iter2 is invalid. it remains invalid
        if (m_iterator_second) {
          ++(*m_iterator_second);
          if (*m_iterator_second == m_pixel_view.end()) {
            m_iterator_second = boost::none;
          }
        }
      }

      // necessary for iterator_facade
      void decrement()
      {
        if (m_iterator_first) {
          if (m_iterator_first != m_pixel_view.begin()) {
            --(*m_iterator_first);
          }
          else {
            m_iterator_first = boost::none;
          }
        }
        
        if (m_iterator_second) {
          --(*m_iterator_second);
        }
        else if (m_iterator_first->get_coordinates().row < size1() -1) {
          // iterator_first will degress from last row to penultimate row.
          // set m_iterator_second to last cell in last row
          m_iterator_second = (*m_iterator_first) + size2();
        }

    
      }
    private:
      template <class, class> friend class h_edge_iterator;

      index_type index() const
      {
        return get_coordinates().row * size2() + get_coordinates().col;
      }

      coordinate_type index_to_coordinate(index_type i) const
      {
        return coordinate_type(i / size2(), i % size2());
      }

  protected:
    template<class OptionalValue>
    void put_iterator_2(OptionalValue& v)
    {
      put_iterator(m_iterator_second, v);
    }

    template<class OptionalValue>
    void put_iterator_1(OptionalValue& v)
    {
      put_iterator(m_iterator_first, v);
    }

    optional_pixel_value get_1() const
    {
      if (m_iterator_first) return **m_iterator_first;
      else return boost::none;
    }
    optional_pixel_value get_2() const
    {
      if (m_iterator_second) return **m_iterator_second;
      else return boost::none;
    }
    
    index_type size1() const // raster size
    {
      return m_pixel_view.size1();
    }

    index_type size2() const // raster size
    {
      return m_pixel_view.size2();
    }

    boost::optional<pixel_iterator> m_iterator_first, m_iterator_second;
    pixel_view m_pixel_view;
  };

  template<typename Raster, typename GetStrategy = get_strategy::both>
  class h_edge_trans_iterator : public detail::edge_iterator_common<
    h_edge_trans_iterator<Raster, GetStrategy>, Raster, GetStrategy>
  {
    using pixel_view = raster_view<orientation::col_major, element::pixel,
      access::read_write, Raster>;

    using pixel_iterator = typename pixel_view::iterator;

  public:
    h_edge_trans_iterator(Raster* r = NULL) : m_pixel_view(r)
    {}

    void find_begin()
    {
      m_iterator_first = boost::none;
      m_iterator_second = m_pixel_view.begin();
    }

    void find_end()
    {
      m_iterator_first = boost::none;
      m_iterator_second = m_pixel_view.end();
    }

    // finds edge above given coordinate
    void find(const coordinate_type& coordinates)
    {
      if (coordinates.row > 0)	{
        if (!m_iterator_first) {
          m_iterator_first = m_pixel_view.begin();
        }
        m_iterator_first->find(coordinates + coordinate_type(-1, 0));
      }
      else {
        m_iterator_first = boost::none;
      }

      if (coordinates.row < size1())	{
        if (!m_iterator_second) {
          m_iterator_second = m_pixel_view.begin();
        }
        m_iterator_second->find(coordinates);
      }
      else {
        m_iterator_second = boost::none;
      }
    }

    coordinate_type get_coordinates() const
    {
      if (m_iterator_second) return m_iterator_second->get_coordinates();
      return m_iterator_first->get_coordinates() + coordinate_type(1, 0);
    }


    // necessary for iterator_facade
    //friend class boost::iterator_core_access;

    // necessary for iterator_facade
    void increment()
    {
      if (!m_iterator_second) {
        m_iterator_second = ++(*m_iterator_first);
        m_iterator_first = boost::none;
      }
      else if (!m_iterator_first)
      {
        m_iterator_first = m_iterator_second;
        ++(*m_iterator_second);
      }
      else{
        m_iterator_first = m_iterator_second;
        ++(*m_iterator_second);
        if (m_iterator_second->get_coordinates().col !=
          m_iterator_first->get_coordinates().col)
        {
          m_iterator_second = boost::none;
        }
      }
    }

    void decrement()
    {
      if (!m_iterator_first) {
        if (*m_iterator_second == m_pixel_view.begin()){
          m_iterator_first = boost::none;
          m_iterator_second = boost::none;
        }
        else {
          m_iterator_first = --(*m_iterator_second);
          m_iterator_second = boost::none;
        }
      }
      else if (m_iterator_first->get_coordinates().row == 0){
        m_iterator_second = m_iterator_first;
        m_iterator_first = boost::none;
      }
      else {
        m_iterator_second = m_iterator_first;
        --(*m_iterator_first);
      }
    }

  private:
    template <class, class> friend class h_edge_trans_iterator;

    index_type index() const
    {
      return get_coordinates().col * (size1() + 1) + get_coordinates().row;
    }

    coordinate_type index_to_coordinate(index_type i) const
    {
      return coordinate_type(i % (size1() + 1), i / (size1() + 1));
    }

  protected:
    template<class OptionalValue>
    void put_iterator_2(OptionalValue& v)
    {
      put_iterator(m_iterator_second, v);
    }

    template<class OptionalValue>
    void put_iterator_1(OptionalValue& v)
    {
      put_iterator(m_iterator_first, v);
    }

    optional_pixel_value get_1() const
    {
      if (m_iterator_first) return **m_iterator_first;
      else return boost::none;
    }
    optional_pixel_value get_2() const
    {
      if (m_iterator_second) return **m_iterator_second;
      else return boost::none;
    }

    index_type size1() const // raster size
    {
      return m_pixel_view.size1();
    }

    index_type size2() const // raster size
    {
      return m_pixel_view.size2();
    }

    boost::optional<pixel_iterator> m_iterator_first, m_iterator_second;
    pixel_view m_pixel_view;
  };


  template<typename Raster, typename GetStrategy = get_strategy::both>
  class v_edge_iterator : public detail::edge_iterator_common<
    v_edge_iterator<Raster, GetStrategy>, Raster, GetStrategy>
  {
    using pixel_view = raster_view<orientation::row_major, element::pixel,
      access::read_only, Raster>;

    using pixel_iterator = typename pixel_view::iterator;

  public:
    v_edge_iterator(Raster* r = NULL) : m_pixel_view(r)
    {}

    void find_begin()
    {
      m_iterator_first = boost::none;
      m_iterator_second = m_pixel_view.begin();
    }

    void find_end()
    {
      m_iterator_first = boost::none;
      m_iterator_second = m_pixel_view.end();
    }


    // finds edge left of given coordinate
    void find(const coordinate_type& coordinates)
    {
      if (coordinates.col > 0)	{
        if (!m_iterator_first)  {
          m_iterator_first = m_pixel_view.begin();
        }
        m_iterator_first->find(coordinates + coordinate_type(0, -1));
      }
      else {
        m_iterator_first = boost::none;
      }

      if (coordinates.col < size2())	{
        if (!m_iterator_second)  {
          m_iterator_second = m_pixel_view.begin();
        }
        m_iterator_second->find(coordinates);
      }
      else {
        m_iterator_second = boost::none;
      }
    }

    // Finds coordinates to right of edge
    coordinate_type get_coordinates() const
    {
      if (m_iterator_second) return m_iterator_second->get_coordinates();
      return m_iterator_first->get_coordinates() + coordinate_type(0, 1);
    }

    // necessary for iterator_facade
    //friend class boost::iterator_core_access;

    // necessary for iterator_facade
    void increment()
    {
      if (m_iterator_first) { // at any time, either iter1 or iter2 is initialized
        ++(*m_iterator_first);
        if (m_iterator_second) {
          ++(*m_iterator_second);
          if (m_iterator_second->get_coordinates().col == 0) {
            m_iterator_second = boost::none;
          }
        }
        else { //!m_iterator_second
          m_iterator_second = m_iterator_first;
          m_iterator_first = boost::none;
        }
      }
      else { //!m_iterator_first
        m_iterator_first = m_iterator_second;
        ++(*m_iterator_second);
        if (m_iterator_second->get_coordinates().col == 0) {
          m_iterator_second = boost::none;
        }
      }
    }

    void decrement()
    {
      if (m_iterator_second) { // at any time, either iter1 or iter2 is initialized
        --(*m_iterator_second);
        if (m_iterator_first) {
          --(*m_iterator_first);
          if (m_iterator_second->get_coordinates().col == 0) {
            m_iterator_first = boost::none;
          }
        }
        else { //!m_iterator_second
          m_iterator_first = m_iterator_second;
          m_iterator_second = boost::none;
        }
      }
      else { //!m_iterator_second
        m_iterator_second = m_iterator_first;
        --(*m_iterator_first);
        if (m_iterator_second->get_coordinates().col == 0) {
          m_iterator_first = boost::none;
        }
      }
    }

  private:
    template <class, class> friend class h_edge_trans_iterator;

    index_type index() const
    {
      return get_coordinates().row * (size2() + 1) + get_coordinates().col;
    }

    coordinate_type index_to_coordinate(index_type i) const
    {
      return coordinate_type(i / (size2() + 1), i % (size2() + 1));
    }

  protected:
    template<class OptionalValue>
    void put_iterator_2(OptionalValue& v)
    {
      put_iterator(m_iterator_second, v);
    }

    template<class OptionalValue>
    void put_iterator_1(OptionalValue& v)
    {
      put_iterator(m_iterator_first, v);
    }

    optional_pixel_value get_1() const
    {
      if (m_iterator_first) return **m_iterator_first;
      else return boost::none;
    }
    optional_pixel_value get_2() const
    {
      if (m_iterator_second) return **m_iterator_second;
      else return boost::none;
    }

    index_type size1() const // raster size
    {
      return m_pixel_view.size1();
    }

    index_type size2() const // raster size
    {
      return m_pixel_view.size2();
    }

    boost::optional<pixel_iterator> m_iterator_first, m_iterator_second;
    pixel_view m_pixel_view;
  };

  template<typename Raster, typename GetStrategy = get_strategy::both>
  class v_edge_trans_iterator : public detail::edge_iterator_common<
    v_edge_trans_iterator<Raster, GetStrategy>, Raster, GetStrategy>
  {
    using pixel_view = raster_view<orientation::col_major, element::pixel,
      access::read_only, Raster>;

    using pixel_iterator = typename pixel_view::iterator;

  public:
    v_edge_trans_iterator(Raster* r = NULL) : m_pixel_view(r)
    {}

    void find_begin()
    {
      m_iterator_first = boost::none;
      m_iterator_second = m_pixel_view.begin();
    }

    void find_end()
    {
      m_iterator_first = m_pixel_view.end();
      m_iterator_second = boost::none;
    }

    // always finds left of given coordinate
    void find(const coordinate_type& coordinates)
    {
      if (coordinates.col > 0)	{
        if (!m_iterator_first) {
          m_iterator_first = m_pixel_view.begin();
        }
        m_iterator_first->find(coordinates + coordinate_type(0, -1));
      }
      else {
        m_iterator_first = boost::none;
      }

      if (coordinates.col < size2())	{
        if (!m_iterator_second) {
          m_iterator_second = m_pixel_view.begin();
        }
        m_iterator_second->find(coordinates);
      }
      else {
        m_iterator_second = boost::none;
      }
    }

    // Finds coordinates to right of edge
    coordinate_type get_coordinates() const
    {
      if (m_iterator_second) return m_iterator_second->get_coordinates();
      return m_iterator_first->get_coordinates() + coordinate_type(0, 1);
    }

    // necessary for iterator_facade
    //friend class boost::iterator_core_access;

    // necessary for iterator_facade
    void increment()
    {
      // Once iter1 is valid. it remains valid
      if (m_iterator_first) {
        ++(*m_iterator_first);
      }
      else if (m_iterator_second->get_coordinates().row == size2()-1)
      {
        m_iterator_first = m_pixel_view.begin();
      }

      if (m_iterator_second) {
        ++(*m_iterator_second);
        if (m_iterator_second == m_pixel_view.end() ) { 
          m_iterator_second = boost::none;
        }
      }

    }

    void decrement()
    {
      // Once iter1 is invalid. it remains invalid
      if (m_iterator_first) {
        if ((*m_iterator_first) != m_pixel_view.begin()) {
          --(*m_iterator_first);
        }
        else {
          m_iterator_first = boost::none;
        }
      }

      // Once iter2 is valid. it remains valid
      if (m_iterator_second) {
        --(*m_iterator_second);
      }
      else if (m_iterator_first->get_coordinates().col == size2()-1) {
        m_iterator_second = (*m_iterator_first) + size2();
      }
    }

  private:
    template <class, class> friend class h_edge_trans_iterator;

    index_type index() const
    {
      return get_coordinates().col * size1() + get_coordinates().row;
    }

    coordinate_type index_to_coordinate(index_type i) const
    {
      return coordinate_type(i % size1(), i / size1());
    }

  protected:
    template<class OptionalValue>
    void put_iterator_2(OptionalValue& v)
    {
      put_iterator(m_iterator_second, v);
    }

    template<class OptionalValue>
    void put_iterator_1(OptionalValue& v)
    {
      put_iterator(m_iterator_first, v);
    }

    optional_pixel_value get_1() const
    {
      if (m_iterator_first) return **m_iterator_first;
      else return boost::none;
    }
    optional_pixel_value get_2() const
    {
      if (m_iterator_second) return **m_iterator_second;
      else return boost::none;
    }

    index_type size1() const // raster size
    {
      return m_pixel_view.size1();
    }

    index_type size2() const // raster size
    {
      return m_pixel_view.size2();
    }

    boost::optional<pixel_iterator> m_iterator_first, m_iterator_second;
    pixel_view m_pixel_view;
  };

 

/*
    namespace raster_traits
    {
      template <typename Raster>
      struct iterator<
        raster_iterator_tag::orientation::row_major,
        raster_iterator_tag::element::h_edge,
        raster_iterator_tag::access::read_only
        , Raster>
      {
        typedef h_edge_iterator<Raster, get_strategy::both> type;
      };

      template <typename Raster>
      struct iterator<
        raster_iterator_tag::orientation::row_major,
        raster_iterator_tag::element::v_edge,
        raster_iterator_tag::access::read_only
        , Raster>
      {
        typedef v_edge_iterator<Raster, get_strategy::both> type;
      };

      template <typename Raster>
      struct iterator<
        raster_iterator_tag::orientation::col_major,
        raster_iterator_tag::element::h_edge,
        raster_iterator_tag::access::read_only
        , Raster>
      {
        typedef h_edge_trans_iterator<Raster, get_strategy::both> type;
      };

      template <typename Raster>
      struct iterator<
        raster_iterator_tag::orientation::col_major,
        raster_iterator_tag::element::v_edge,
        raster_iterator_tag::access::read_only
        , Raster>
      {
        typedef v_edge_trans_iterator<Raster, get_strategy::both> type;
      };

      template <typename Raster>
      struct iterator<
        raster_iterator_tag::orientation::row_major,
        raster_iterator_tag::element::h_edge_first_only,
        raster_iterator_tag::access::read_only
        , Raster>
      {
        typedef h_edge_iterator<Raster, get_strategy::first_only> type;
      };

      template <typename Raster>
      struct iterator<
        raster_iterator_tag::orientation::row_major,
        raster_iterator_tag::element::v_edge_first_only,
        raster_iterator_tag::access::read_only
        , Raster>
      {
        typedef v_edge_iterator<Raster, get_strategy::first_only> type;
      };

      template <typename Raster>
      struct iterator<
        raster_iterator_tag::orientation::col_major,
        raster_iterator_tag::element::h_edge_first_only,
        raster_iterator_tag::access::read_only
        , Raster>
      {
        typedef h_edge_trans_iterator<Raster, get_strategy::first_only> type;
      };

      template <typename Raster>
      struct iterator<
        raster_iterator_tag::orientation::col_major,
        raster_iterator_tag::element::v_edge_first_only,
        raster_iterator_tag::access::read_only
        , Raster>
      {
        typedef v_edge_trans_iterator<Raster, get_strategy::first_only> type;
      };

      template <typename Raster>
      struct iterator<
        raster_iterator_tag::orientation::row_major,
        raster_iterator_tag::element::h_edge_second_only,
        raster_iterator_tag::access::read_only
        , Raster>
      {
        typedef h_edge_iterator<Raster, get_strategy::second_only> type;
      };

      template <typename Raster>
      struct iterator<
        raster_iterator_tag::orientation::row_major,
        raster_iterator_tag::element::v_edge_second_only,
        raster_iterator_tag::access::read_only
        , Raster>
      {
        typedef v_edge_iterator<Raster, get_strategy::second_only> type;
      };

      template <typename Raster>
      struct iterator<
        raster_iterator_tag::orientation::col_major,
        raster_iterator_tag::element::h_edge_second_only,
        raster_iterator_tag::access::read_only
        , Raster>
      {
        typedef h_edge_trans_iterator<Raster, get_strategy::second_only> type;
      };

      template <typename Raster>
      struct iterator<
        raster_iterator_tag::orientation::col_major,
        raster_iterator_tag::element::v_edge_second_only,
        raster_iterator_tag::access::read_only
        , Raster>
      {
        typedef v_edge_trans_iterator<Raster, get_strategy::second_only> type;
      };

    } // namespace raster_traits
    */
  } // namespace 
} // namespace 
#endif