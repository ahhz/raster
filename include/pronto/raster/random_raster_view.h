//=======================================================================
// Copyright 2015-2107
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
// TODO: at the moment every random raster has its own cache. 
// This can become expense, a basic improvement would be to let at least have 
// subrasters of the same master raster use the same cache.
// Even better would be to join the cache already used by GDAL, perhaps even 
// propose the random_raster as a special kind of GDAL raster.

#pragma once

#include <pronto/raster/optional.h>
#include <pronto/raster/traits.h>

#include <array>
#include <cassert>
#include <iterator>
#include <list>
#include <memory>
#include <random> 

namespace pronto
{
  namespace raster
  {
    template<class Distribution, class Generator
      , int RowsInBlock, int ColsInBlock>
      class cached_random_blocks
    {
    public:
      using value_type = typename Distribution::result_type;
      using seed_type = typename Generator::result_type;


      int block_rows() const
      {
        return RowsInBlock;
      }
      int block_cols() const
      {
        return ColsInBlock;
      }

    private:
      using data = std::array<value_type, RowsInBlock * ColsInBlock>;
      struct cached_block; // forward declaratiom
      using lru_list = std::list<cached_block>;
      using lru_iterator = typename lru_list::iterator;
      using lru_const_iterator = typename lru_list::const_iterator;

      struct cached_block
      {
        int m_lock_count;
        int m_block_index;
        data m_data;
      };

      struct block
      {
        seed_type m_seed;
        optional<lru_iterator> m_cached_block;
      };

    public:
      class block_access
      {
      public:
        block_access(lru_iterator i) : m_iter(i)
        {}

        block_access() = default;
        block_access(const block_access&) = default;
        block_access(block_access&&) = default;
        block_access& operator=(const block_access&) = default;
        block_access& operator=(block_access&&) = default;
        ~block_access() = default;

        using iterator = typename data::iterator;
        typename iterator begin() const
        {
          return m_iter->m_data.begin();
        }

        typename iterator end() const
        {
          return m_iter->m_data.end();
        }

        void add_lock()
        {
          ++(m_iter->m_lock_count);
        }

        void drop_lock()
        {
          --(m_iter->m_lock_count);
        }
        bool is_locked() const
        {
          return (m_iter->m_lock_count) > 0;
        }
        
        int rows() const
        {
          return RowsInBlock;
        }

        int cols() const
        {
          return ColsInBlock;
        }

        int index() const
        {
          return m_iter->m_block_index;
        }

      private:
        lru_iterator m_iter;
      };
    public:

      class iterator
      {
        using view_type = cached_random_blocks;

        using block_type = typename view_type::block_access;
        using block_iterator_type = typename block_type::iterator;

      public:
        using is_mutable = std::bool_constant<false>;

        using value_type = typename view_type::value_type;
        using reference = const value_type&;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using iterator_category = std::input_iterator_tag;

        iterator()
          : m_block()
          , m_end_of_stretch() // not so elegant
          , m_pos()            // not so elegant
        {}

        iterator(const iterator& other) = default;
        iterator(iterator&& other) = default;
        iterator& operator=(const iterator& other)
          = default;
        iterator& operator=(iterator&& other) = default;
        ~iterator() = default;

        friend inline bool operator==(const iterator& a
          , const iterator& b)
        {
          return a.m_block->index() == b.m_block->index() && a.m_pos == b.m_pos;
          // compare pointers to elements because cannot compare iterators 
          // from different arrays 
        }

        friend inline bool operator!=(const iterator& a
          , const iterator& b)
        {
          return !(a==b);
          // compare pointers to elements because cannot compare iterators 
          // from different arrays 
        }

        inline iterator& operator+=(std::ptrdiff_t distance)
        {
          goto_index(get_index() + static_cast<int>(distance));
          return *this;
        }

        inline iterator& operator-=(std::ptrdiff_t distance)
        {
          goto_index(get_index() - static_cast<int>(distance));
          return *this;
        }

        inline iterator& operator--()
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

        inline iterator& operator--(int)
        {
          iterator temp(*this);
          --(*this);
          return temp;
        }

        inline iterator operator+(std::ptrdiff_t distance) const
        {
          iterator temp(*this);
          temp += distance;
          return temp;
        }

        inline iterator operator-(std::ptrdiff_t distance) const
        {
          iterator temp(*this);
          temp += distance;
          return temp;
        }

        inline reference operator[](std::ptrdiff_t distance) const
        {
          return *(operator+(distance));
        }

        inline bool operator<(const iterator& that) const
        {
          return get_index() < that.get_index();
        }

        inline bool operator>(const iterator& that) const
        {
          return get_index() > that.get_index();
        }

        inline bool operator<=(const iterator& that) const
        {
          return get_index() <= that.get_index();
        }

        inline bool operator>=(const iterator& that) const
        {
          return get_index() >= that.get_index();
        }

        inline iterator& operator++()
        {
          ++m_pos;

          if (m_pos == m_end_of_stretch) {
            --m_pos;
            return goto_index(get_index() + 1);
          }
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
          return *m_pos;
        }

      private:
        friend class view_type;

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

          //     int major_row = m_block.major_row();
          //     int major_col = m_block.major_col();

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

        iterator& goto_index(int index)
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

          int blocks_per_row = m_view->m_full_cols / block_cols;

          int major_index = m_major_row * blocks_per_row + m_major_col;

          int row_in_block = full_row % block_rows;
          int col_in_block = full_col % block_cols;

          int index_in_block = row_in_block * block_cols + col_in_block;

          //m_block.drop_lock();
          auto lock_dropper = [](block_type* b) {b->drop_lock(); delete b;  };
          m_block.reset(new block_type(m_view->get_block(major_index)), lock_dropper);
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


      cached_random_blocks(int rows, int cols, int num_blocks
        , Distribution distribution
        , Generator generator)
        :  m_rows(rows), m_cols(cols), m_distribution(distribution)
        , m_max_blocks_in_memory(num_blocks)
      {
        int block_rows = (RowsInBlock + m_rows - 1) / RowsInBlock;
        int block_cols = (ColsInBlock + m_cols - 1) / ColsInBlock;
        int total_blocks = block_rows * block_cols;
        m_blocks.resize(total_blocks);
        for (auto&& b : m_blocks) {
          b.m_seed = generator();
        }
      }
      cached_random_blocks() = default;
      cached_random_blocks(const cached_random_blocks&) = default;
      cached_random_blocks(cached_random_blocks&&) = default;
      cached_random_blocks& operator=(const cached_random_blocks&) = default;
      cached_random_blocks& operator=(cached_random_blocks&&) = default;


      block_access get_block(int index) const
      {
        block& b = m_blocks[index];
        if (b.m_cached_block) {
          m_lru.splice(m_lru.end(), m_lru, *b.m_cached_block);
        }
        else {
          fill_one_block(index);
        }
        
        return block_access(--(m_lru.end()));
      }

    private:
      lru_iterator clear_one_block()  const
      {
        for (auto i = m_lru.begin(); i != m_lru.end(); ++i) {
          int index = i->m_block_index;
          block& bl = m_blocks[index];
          bl.m_cached_block = none;

          if (!(i->m_lock_count > 0))
          {
             m_lru.splice(m_lru.end(), m_lru, i);
            return i;
          }
        }
        throw("cannot clear one block, all are locked");
        return m_lru.end();
      }

      lru_iterator add_one_block()  const
      {
         m_lru.emplace_back(cached_block{});
        return --(m_lru.end());
      }

      void fill_one_block(int index)  const
      {
        lru_iterator i;
        if (static_cast<int>(m_lru.size()) < m_max_blocks_in_memory) {
           i = add_one_block();
        } 
        else {
          i = clear_one_block();
        }

        i->m_block_index = index;

        Generator rng(m_blocks[index].m_seed);
        for (auto&& j : i->m_data) {
          j = m_distribution(rng);
        }
        m_blocks[index].m_cached_block = i;
      }

    public:

      iterator begin() const
      {
        iterator i;
        i.find_begin(this);
        return i;
      }
	  
      iterator end() const
      {
        iterator i;
        i.find_end(this);
        return i;
      }
	  
      cached_random_blocks sub_raster(int first_row, int first_col, int rows, int cols)
      {
        cached_random_blocks copy = *this;
        for (auto&& i : copy.m_blocks)
        {
          i.m_cached_block = none;
        }
        copy.m_lru.clear();
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
      mutable Distribution m_distribution;
      mutable std::vector<block> m_blocks;
      mutable lru_list m_lru;
      int m_max_blocks_in_memory;
      int m_rows; // only the subset rows
      int m_cols; // only the subset rows
      int m_first_row; // first row in subset
      int m_first_col; // first col in subset
      int m_full_rows; // full number of rows, before taking subset
      int m_full_cols; // full number of rows, before taking subset
    };

    
    // Default block size is 128 x 128
    // Default generator is mersenne twister, seeded by time
    // Default memory allowance is enough for two rows of blocks
    template<class Distribution, class Generator = std::mt19937_64
      , int BlockRows = 128, int BlockCols = 128>
    cached_random_blocks<Distribution, Generator, BlockRows, BlockCols>
      random_distribution_raster(int rows, int cols
        , Distribution dist
        , Generator gen = Generator(std::random_device()()))
    {
      int two_rows_of_blocks = 2 * (BlockCols + cols - 1) / BlockCols;
      return cached_random_blocks<Distribution, Generator
        , BlockRows, BlockCols>
        (rows, cols, two_rows_of_blocks, dist, gen);
    }
  }
}
