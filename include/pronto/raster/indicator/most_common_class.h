
//=======================================================================
// Copyright 2015
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================

#pragma once

#include <pronto/raster/traits.h>
#include <pronto/raster/optional.h>

#include <iterator>
#include <utility>
#include <vector>
#include <map>
#include <unordered_map>

namespace pronto {
  namespace raster {

    
    // This class may be more useful in general, but only completed for the 
    // necessary functionality in the most_common_class indicator
    template<class Key, class Value, class Compare>
    class value_sorted_map
    {
    private:
      using multimap = std::multimap<const Value, const Key, Compare>;
      using mm_iterator = typename multimap::const_iterator;
   
    public:

      value_sorted_map() = default;
      value_sorted_map(const value_sorted_map& vsm)
      {
        m_sorted_by_values = vsm.m_sorted_by_values;
        make_unsorted_from_sorted();
      }

      value_sorted_map(value_sorted_map&& vsm)
      {
        m_sorted_by_values = std::move(vsm.m_sorted_by_values);
        make_unsorted_from_sorted();
      }

      value_sorted_map& operator=(const value_sorted_map& vsm)
      {
        m_sorted_by_values = vsm.m_sorted_by_values; 
        make_unsorted_from_sorted();
        return *this;
      }

      value_sorted_map& operator=(value_sorted_map&& vsm)
      {
        m_sorted_by_values = std::move(vsm.m_sorted_by_values);
        make_unsorted_from_sorted();
        return *this;
      }
      
      class iterator
      {
      public: 
        using value_type = std::pair<const Key&, const Value&>;
        using reference = value_type;

        using difference_type = void;
        using pointer = void;
        using iterator_category = std::input_iterator_tag;

        iterator(const mm_iterator& iter) : m_iter(iter)
        {};
        iterator() = default;
        iterator(const iterator& other) = default;
        iterator(iterator&& other) = default;
        iterator& operator=(const iterator& other)
          = default;
        iterator& operator=(iterator&& other) = default;
        ~iterator() = default;

        friend inline bool operator==(const iterator& a
          , const iterator& b)
        {
          return a.m_iter == b.m_iter;
        }

        friend inline bool operator!=(const iterator& a
          , const iterator& b)
        {
          return a.m_iter != b.m_iter;
        }

        iterator& operator++()
        {          
          ++m_iter;
          return *this;
        }
        
        iterator operator++(int)
        {
          iterator temp(*this);
          ++(*this);
          return temp;
        }

        inline reference operator*() const
        {
          return value_type(m_iter->second, m_iter->first);
        }
       
      public:
        mm_iterator m_iter;
      };

      void add(const Key& k, const Value& v)
      {
        auto i = m_unsorted_map.find(k);
        if (i != m_unsorted_map.end())
        {
          auto old_j = i->second;
          auto old_value = old_j->first;
          m_sorted_by_values.erase(old_j);
          auto new_j = m_sorted_by_values.insert(std::make_pair(old_value+v , k));
          i->second = new_j;
        }
        else {
          auto new_j = m_sorted_by_values.insert(std::make_pair(v, k));
          m_unsorted_map.insert(std::make_pair(k, new_j));
        }
      }

      void subtract(const Key& k, const Value& v)
      {
        auto i = m_unsorted_map.find(k);
        if (i != m_unsorted_map.end())
        {
          auto old_j = i->second;
          auto old_value = old_j->first;
          m_sorted_by_values.erase(old_j);
          auto new_j = m_sorted_by_values.insert(std::make_pair(old_value-v, k));
          i->second = new_j;
        }
        else {
          assert(false);
          auto new_j = m_sorted_by_values.insert(std::make_pair(-v, k));
          m_unsorted_map.insert(std::make_pair(k, new_j));
        }
      }

      std::pair<const Key&, const Value&>  front() const
      {
        return std::pair<const Key&, const Value&>(
          m_sorted_by_values.begin()->second,
          m_sorted_by_values.begin()->first);
      }

      iterator begin() const
      {
        return iterator(m_sorted_by_values.begin());
      }

      iterator end() const
      {
        return iterator(m_sorted_by_values.end());
      }
      bool empty() const
      {
        return m_unsorted_map.empty();
      }

    private:
    
      void make_unsorted_from_sorted()
      {
        m_unsorted_map.clear();
        auto i = m_sorted_by_values.begin();
        auto end = m_sorted_by_values.end();

        for (;i != end; ++i) {
          m_unsorted_map.insert(std::make_pair(i->second, i));
        }
      }
      multimap m_sorted_by_values;
      std::unordered_map<Key, mm_iterator> m_unsorted_map;

    };

    template<typename T, typename Weight = double>
    struct most_common_class
    {
      typedef T value_type;

      using vsm = value_sorted_map<T, Weight, std::greater<T> >;
      void add_sample(const value_type& v, const Weight& w)
      {
        m_counts.add(v, w);
      }

      void add_sample(const value_type& v)
      {
        m_counts.add(v, 1);
      }
  
      void subtract_sample(const value_type& v, const Weight& w)
      {
        m_counts.subtract(v, w);
      }

      void subtract_sample(const value_type& v)
      {
        m_counts.subtract(v, 1);

      }

      template<typename Weight>
      void add_subtotal(const most_common_class& subtotal, const Weight& w)
      {
        for (auto&& i : *subtotal.m_counts) {
          m_counts.add(i.first,i.second * w);
        }
      }

      void add_subtotal(const most_common_class& subtotal)
      {
        for (auto&& i : *subtotal.m_counts) {
          m_counts.add(i.first, i.second);
        }
      }

      template<typename Weight>
      void subtract_subtotal(const most_common_class& subtotal, const Weight& w)
      {
        for (auto&& i : *subtotal.m_counts) {
          m_counts.subtract(i.first, i.second * w);
        }
      }

      void subtract_subtotal(const most_common_class& subtotal)
      {
        for (auto&& i : *subtotal.m_counts) {
          m_counts.subtract(i.first, i.second);
        }
      }

      std::optional<T> extract() const
      {
        if (m_counts.empty()) return std::nullopt;
        return m_counts.front().first;
      }
      
      value_sorted_map<T, Weight, std::greater<Weight> > m_counts;

    };

    template<class T>
    class most_common_class_generator
    {
    public:
      using indicator = most_common_class<T>;
      indicator operator()() const
      {
        return indicator{};
      }
    };
  } 
}
