//
//=======================================================================
// Copyright 2016-2018
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once

#include <pronto/raster/optional.h>
#include <pronto/raster/exceptions.h>

#include <list>
#include <functional>

// inlined all member functions which is not so elegant. Just to keep this a 
// header only library..., need to reconsider.

namespace pronto {
  namespace raster {
    
    class lru
    {
      struct element
      {

        using id = typename std::list<element>::iterator;
        using id2 = typename std::list<id>::iterator;

        element(std::size_t size, std::function<void()> closer)
          : m_size(size), m_lock_count(0), m_on_close(closer)
        {}

        inline std::size_t size() const  { return m_size; };
        inline bool is_locked() const { return m_lock_count > 0; }
        inline void close() const { return m_on_close(); }
        inline id2 get_iter_in_unlocked_list() { return *m_iter_in_unlocked_list; }

        inline void add_lock() { ++m_lock_count; }
        inline void drop_lock() { --m_lock_count; }
        
        inline void set_iter_in_list(id i) { m_iter_in_list = i;  }
        inline void set_iter_in_unlocked_list(id2 i) { m_iter_in_unlocked_list = i; }
        inline void clear_iter_in_unlocked_list() { m_iter_in_unlocked_list = none; }
     
      private:
        std::function<void()> m_on_close;
        int m_lock_count;
        id m_iter_in_list;
        optional<id2> m_iter_in_unlocked_list;
        std::size_t m_size;
      };
    public:

      lru(std::size_t capacity = static_cast<std::size_t>(1e6)) : m_capacity(capacity), m_total_size(0)
      {}

      ~lru()
      {
        assert(m_all.empty() == (m_total_size == 0));

        auto i = m_all.begin();
        while (i != m_all.end())
        {
          auto j = i;
          ++i;
          remove(j);
        }
        assert(m_all.empty() == (m_total_size == 0));
        assert(m_all.empty() && m_unlocked.empty());
      }
      using id = typename std::list<element>::iterator;
    
      inline std::size_t get_capacity() const
      {
        return m_capacity;
      }
      inline void set_capacity(std::size_t  cap)
      {
        m_capacity = cap;
      }
      inline std::size_t get_size() const
      {
        return m_total_size;
      }
  

    private:
      using id2 = typename std::list<id>::iterator;



      inline void make_space(std::size_t size)
      {
        while (m_total_size + size > m_capacity)
        {
          // close first locked element
          // remove from both lists

          if (m_unlocked.empty())
          {

            throw(lru_full{});

          }
          m_total_size -= m_unlocked.front()->size();
          m_unlocked.front()->close();
          m_all.erase(m_unlocked.front());
          m_unlocked.erase(m_unlocked.begin());
        }
      }
    public:
      inline id add(std::size_t size, std::function<void()> on_close)
      {
        make_space(size);
        m_total_size += size;
        m_all.emplace_back(size, on_close);
        id element = std::prev(m_all.end());
        element->set_iter_in_list(element);
        m_unlocked.emplace_back(element);
        element->set_iter_in_unlocked_list(std::prev(m_unlocked.end()));
        return element;
      }

      inline id add_with_lock(std::size_t size, std::function<void()> on_close)
      {
        make_space(size);
        m_total_size += size;
        m_all.emplace_back(size, on_close);
        id element = std::prev(m_all.end());
        element->set_iter_in_list(element);
        element->add_lock();
        return element;
      }

      inline void add_lock(id element)
      {
        bool was_locked = element->is_locked();
        element->add_lock();
        if(!was_locked) {
          // Just became locked, remove from unlocked list
          //
          m_unlocked.erase( element->get_iter_in_unlocked_list() );
          element->clear_iter_in_unlocked_list();
        }
      }
      inline void drop_lock(id element)
      {
        if (!element->is_locked())
        {
          throw("lru: logical error, trying to drop a lock of an unlocked element");

        }
        element->drop_lock();
        
        if (!element->is_locked())
        {
          // Just became unlocked, add to unlocked list
     
          // find first unlocked element that is more recently used
          auto is_unlocked = [](lru::element& elem) {return !elem.is_locked(); };
          id next_elem = std::find_if(std::next(element), m_all.end(), is_unlocked);

          // find corresponding in m_unlocked
          id2 next_elem2 = m_unlocked.end();
          if (next_elem != m_all.end()) {
            next_elem2 = next_elem->get_iter_in_unlocked_list();
          }

          // put directly before this in unlocked list
          auto new_elem = m_unlocked.insert(next_elem2, element);
          element->set_iter_in_unlocked_list(new_elem);
         }
      }
      inline void remove(id element)
      {
        // removes regardless of locks
        assert(!element->is_locked());
        m_total_size -= element->size();
        element->close();
        if (!element->is_locked())
        {
          id2 elem2 = element->get_iter_in_unlocked_list();
          m_unlocked.erase(elem2);
        }
        m_all.erase(element);
      }

      bool empty()
      {
        assert(m_all.empty() == (m_total_size == 0));
        return m_all.empty();
      }

      inline void touch(id element)
      {
        // move to back
        m_all.splice(m_all.end(), m_all, element);
        
        // if unlocked also move to back in unlocked list
        if (!element->is_locked()) {
          id2 e2 = element->get_iter_in_unlocked_list();
          m_unlocked.splice(m_unlocked.end(), m_unlocked, e2);
        }
      }

    private:
      std::list<element> m_all;
      std::list<id> m_unlocked;
      std::size_t m_total_size;
      std::size_t m_capacity;
    };

    extern lru g_lru;
  }
}