//=======================================================================
// Copyright 2015-2107
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
// 

#pragma once

#include <pronto/raster/optional.h>
#include <pronto/raster/traits.h>
#include <pronto/raster/lru.h>
#include <pronto/raster/reference_proxy.h>
#include <array>
#include <cassert>
#include <iterator>
#include <list>
#include <memory>
#include <random> 
#include <vector> 

namespace pronto
{
  namespace raster
  {
    extern lru g_lru;
    
    template<class T>
    class data_block
    {
    public:
      using handle_type = typename lru::id;
      using value_type = T;
      using iterator = typename std::vector<value_type>::iterator;
      using const_iterator = typename std::vector<value_type>::const_iterator;
    
      ~data_block()
      {
        // when a data block is deleted and it still is in the lru, remove it
        // this will also cause the vector to clear and shrink
        if (has_data()) remove_from_lru();
      }

      const_iterator begin() const 
      {
        assert(has_data());
        return m_data.begin();
      }

      const_iterator end() const 
      {
        assert(has_data());
        return m_data.end();
      }

      iterator begin()
      {
        assert(has_data());
        return m_data.begin();
      }

      iterator end()
      {
        assert(has_data());
        return m_data.end();
      }
           
      bool has_data() const
      {
        return m_handle.has_value();
      }
         
      void drop_lock()
      {
        m_lru.drop_lock(*m_handle);
      }

      void add_lock()
      {
        m_lru.add_lock(*m_handle);
      }

      void touch()
      {
        m_lru.touch(*m_handle);
      }
      std::vector<T>& get_vector()
      {
        return m_data;
      }
    
      void create_data(std::size_t size)
      {
        assert(!m_handle);
        auto closer = [this]() { clear(); };
        m_handle = m_lru.add(size * sizeof(T), closer);
        m_data.resize(size);
      }
      void set_pre_clear(std::function<void()> f)
      {
        m_pre_clear = f;
      }

 
       void clear()
      {
        if(m_pre_clear) (*m_pre_clear)();
        m_pre_clear = none;
        m_handle = none;
        m_data.clear();
        m_data.shrink_to_fit();
      }
    private:

      void remove_from_lru()
      {
        m_lru.remove(*m_handle);// will cause clear() through closer()
      }
      optional<handle_type> m_handle;
      std::vector<value_type> m_data;
      optional<std::function<void()> > m_pre_clear;
      lru& m_lru = g_lru; // still using the global lru but set up to use any
    };

    template<class T, class Block>
    class blocked_reference
    {
    public:
      using value_type = T;
      blocked_reference(T& ref, Block* block) : m_reference(ref), m_block(block)
      {
        assert(block);
        block->add_lock();
      }
      blocked_reference(const blocked_reference& that) 
        : m_reference(that.m_reference)
        , m_block(that.m_block)
      {
        if(m_block) m_block->add_lock();
      }

      blocked_reference(blocked_reference&& that)
        : m_reference(that.m_reference)
        , m_block(that.m_block)
      {
        that.m_block = nullptr;
      }


      ~blocked_reference()
      {
        if(m_block) m_block->drop_lock();
      }

      value_type get() const
      {
        return m_reference;
      }

      void put(const value_type& value) 
      {
        m_reference = value;
      }

    private:

      value_type& m_reference;
      Block* m_block;
    };

    template<class CachedBlockRasterView, bool IsMutable, bool IsForwardOnly>
    class cached_block_raster_iterator
    {
      using view_type = CachedBlockRasterView;
      using block_provider_type =
        typename CachedBlockRasterView::block_provider_type;
      
      using block_type = typename block_provider_type::block;
      using block_iterator_type = typename block_type::iterator;
      using this_type = cached_block_raster_iterator;
    public:
      using is_mutable = std::bool_constant<IsMutable>;
      using is_forward_only = std::bool_constant<IsForwardOnly>;

          

    public:
    
      using value_type = typename view_type::value_type;
      using proxy_reference = reference_proxy < blocked_reference<value_type, block_type> >;

      // when forward only, the dereferenced iterator does not need to outlive 
      // the iterator, so can return references. 
      // when non-mutable we can return a value instead of a reference, 
      // when mutable and not forward-only, we need a proxy_reference 

      using forward_only_reference = typename
        std::conditional<IsMutable, value_type&, const value_type&>::type;

      using non_forward_only_reference = typename 
        std::conditional<IsMutable, proxy_reference, value_type>::type;

      using reference = typename std::conditional<IsForwardOnly,
        forward_only_reference, non_forward_only_reference>::type;

      using difference_type = std::ptrdiff_t;
      using pointer = void;
      using iterator_category = std::input_iterator_tag;

      cached_block_raster_iterator()
        : m_block()
        , m_end_of_stretch() // not so elegant
        , m_pos()            // not so elegant
      {}

      cached_block_raster_iterator(const this_type& other) = default;
      cached_block_raster_iterator(this_type&& other) = default;
      cached_block_raster_iterator& operator=(const this_type& other)
        = default;
      cached_block_raster_iterator& operator=(this_type&& other) = default;
      ~cached_block_raster_iterator() = default;

      friend inline bool operator==(const this_type& a
        , const this_type& b)
      {
        return a.m_block == b.m_block && a.m_pos == b.m_pos;
      }

      friend inline bool operator!=(const this_type& a
        , const this_type& b)
      {
        return !(a == b);
      }

      inline this_type& operator+=(std::ptrdiff_t distance)
      {
        goto_index(get_index() + static_cast<int>(distance));
        return *this;
      }

      inline this_type& operator-=(std::ptrdiff_t distance)
      {
        goto_index(get_index() - static_cast<int>(distance));
        return *this;
      }

      inline this_type& operator--()
      {
        auto d = std::distance(m_block->begin(), m_pos);
        if (d % m_block->block_cols() > 0) {
          --m_pos;
          return *this;
        }
        else
        {
          return goto_index(get_index() - 1);
        }
      }

      inline this_type& operator--(int)
      {
        this_type temp(*this);
        --(*this);
        return temp;
      }

      inline this_type operator+(std::ptrdiff_t distance) const
      {
        this_type temp(*this);
        temp += distance;
        return temp;
      }

      inline this_type operator-(std::ptrdiff_t distance) const
      {
        this_type temp(*this);
        temp += distance;
        return temp;
      }

      inline reference operator[](std::ptrdiff_t distance) const
      {
        return *(operator+(distance));
      }

      inline bool operator<(const this_type& that) const
      {
        return get_index() < that.get_index();
      }

      inline bool operator>(const this_type& that) const
      {
        return get_index() > that.get_index();
      }

      inline bool operator<=(const this_type& that) const
      {
        return get_index() <= that.get_index();
      }

      inline bool operator>=(const this_type& that) const
      {
        return get_index() >= that.get_index();
      }

      inline this_type& operator++()
      {
        ++m_pos;

        if (m_pos == m_end_of_stretch) {
          --m_pos;
          return goto_index(get_index() + 1);
        }
        return *this;
      }

      this_type operator++(int)
      {
        this_type temp(*this);
        ++(*this);
        return temp;
      }

      using is_mutable = std::bool_constant<IsMutable>;
      using is_forward_only = std::bool_constant<IsForwardOnly>;
    
      inline reference get_reference(std::true_type, std::true_type) const
      {
        return *m_pos;
      }

      inline reference get_reference(std::true_type, std::false_type) const
      {
        blocked_reference<value_type, block_type> ref(*m_pos, m_block.get());
        return proxy_reference(ref);
      }

      inline reference get_reference(std::false_type, std::true_type) const
      {
        return *m_pos;
      }

      inline reference get_reference(std::false_type, std::false_type) const
      {
        return *m_pos;
      }



      inline reference operator*() const
      {
        return get_reference(is_mutable{}, is_forward_only{});
      }

    public:
//      friend class CachedBlockRasterView;

      void find_begin(const view_type* view)
      {
        m_view = view;
        goto_index(0);
      }

      void find_end(const view_type* view)
      {
        m_view = view;
        goto_index(m_view->rows() * m_view->cols());
      }

    private:

      int get_index() const
      {
        // it might seem more efficient to just add an index member to the 
        // iterator, however the hot-path is operator++ and operator*(), 
        // keep those as simple as possible 

        int block_rows = m_view->block_rows();
        int block_cols = m_view->block_cols();

        block_iterator_type start = m_block->begin();

        int index_in_block = static_cast<int>(std::distance(start, m_pos));
        assert(index_in_block >= 0);

        int minor_row = index_in_block / block_cols;
        int minor_col = index_in_block % block_cols;

        int full_row = m_major_row * block_rows + minor_row;
        int full_col = m_major_col * block_cols + minor_col;

        int row = full_row - m_view->m_first_row;
        int col = full_col - m_view->m_first_col;

        // in last block? one past the last element?
        if (row == m_view->rows() || col == m_view->cols()) {
          return m_view->rows() * m_view->cols();
        }
        else {
          return row * m_view->cols() + col;
        }
      }

      this_type& goto_index(int index)
      {
        if (index == m_view->cols() * m_view->rows()) {
          if (index == 0) { // empty raster, no place to go
            m_pos = block_iterator_type{};
            return *this;
          }
          // Go to last block, one past the last element.
          goto_index(index - 1);
          ++m_pos;
          return *this;
        }

        int row = index / m_view->cols();
        int col = index % m_view->cols();

        int full_row = row + m_view->m_first_row;
        int full_col = col + m_view->m_first_col;

        int block_rows = m_view->block_rows();
        int block_cols = m_view->block_cols();

        m_major_row = full_row / block_rows;
        m_major_col = full_col / block_cols;

        int blocks_per_row = 1 + (m_view->m_block_provider->cols() - 1) / block_cols;

        int major_index = m_major_row * blocks_per_row + m_major_col;

        int row_in_block = full_row % block_rows;
        int col_in_block = full_col % block_cols;

        int index_in_block = row_in_block * block_cols + col_in_block;

        auto dropper = [](block_type* b) { 
          b->drop_lock(); 
        };
        auto block = m_view->m_block_provider->get_block(major_index);
        m_block.reset(block, dropper);
        m_block->add_lock();

        m_pos = m_block->begin() + index_in_block;

        int end_col = std::min<int>((m_major_col + 1) * block_cols
          , m_view->m_first_col + m_view->m_cols);

        int minor_end_col = 1 + (end_col - 1) % block_cols;
        int end_of_stretch_index = row_in_block * block_cols + minor_end_col;

        m_end_of_stretch = m_block->begin() + end_of_stretch_index;
        assert(m_pos != m_end_of_stretch);
        return *this;
      }

      const view_type* m_view;
      int m_major_row;
      int m_major_col;

      std::shared_ptr<block_type> m_block;
      block_iterator_type m_end_of_stretch;
      block_iterator_type m_pos;
    };


    template<class BlockProvider, bool IsMutable,  bool IsForwardOnly>
    class cached_block_raster_view
    {
    public:
      using is_mutable = std::bool_constant<IsMutable>;
      using is_forward_only = std::bool_constant<IsForwardOnly>;

      using block_provider_type = BlockProvider;
      using value_type = typename BlockProvider::value_type;

      using const_iterator = cached_block_raster_iterator
        <cached_block_raster_view, false, IsForwardOnly>;
      
      using iterator = cached_block_raster_iterator
        <cached_block_raster_view, IsMutable, IsForwardOnly>;

      int block_rows() const
      {
        return m_block_provider->block_rows();
      }
      int block_cols() const
      {
        return m_block_provider->block_cols();
      }

    public:
      cached_block_raster_view(std::shared_ptr<BlockProvider> bp)
        : m_block_provider(bp), m_rows(bp->rows()), m_cols(bp->cols())
     //   , m_full_cols(bp->cols()), m_full_rows(bp->rows())
      { }

      cached_block_raster_view() = default;
      cached_block_raster_view(const cached_block_raster_view&) = default;
      cached_block_raster_view(cached_block_raster_view&&) = default;
      cached_block_raster_view& operator=(const cached_block_raster_view&) = default;
      cached_block_raster_view& operator=(cached_block_raster_view&&) = default;

    public:

      const_iterator begin() const
      {
        const_iterator i;
        i.find_begin(this);
        return i;
      }
	  
      const_iterator end() const
      {
        const_iterator i;
        i.find_end(this);
        return i;
      }

      iterator begin() 
      {
        iterator i;
        i.find_begin(this);
        return i;
      }

      iterator end() 
      {
        iterator i;
        i.find_end(this);
        return i;
      }
	  
      cached_block_raster_view sub_raster(int first_row, int first_col, int rows, int cols) const
      {
        cached_block_raster_view copy = *this;
        copy.m_first_row = m_first_row + first_row;
        copy.m_first_col = m_first_col + first_col;
        copy.m_rows = rows;
        copy.m_cols = cols;
        return copy;
      }

      int rows() const
      {
        return m_rows;
      }
	  
      int cols() const
      {
        return m_cols;
      }

      int size() const
      {
        return m_rows * m_cols;
      }
	  
    private:
      friend class iterator;
      friend class const_iterator;
      std::shared_ptr<BlockProvider> m_block_provider;
      int m_rows; // only the subset rows
      int m_cols; // only the subset rows
      int m_first_row; // first row in subset
      int m_first_col; // first col in subset
     };
  }
}
