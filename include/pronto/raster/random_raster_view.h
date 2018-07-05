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
    template<class T>
    class data_block
    {
    public:
      using handle_type = typename lru::id;
      using value_type = T;
      using iterator = typename std::vector<value_type>::iterator;
      using const_iterator = typename std::vector<value_type>::const_iterator;
    

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

      handle_type get_handle() const
      {
        return *m_handle;
      }

      bool has_data() const
      {
        return m_handle.has_value();
      }

      void clear()
      {
        m_handle = none;
        m_data.clear();
        m_data.shrink_to_fit();
      }

      void set_handle_and_resize(handle_type handle, std::size_t size)
      {
        m_handle = handle;
        m_data.resize(size);
      }

    private:
      optional<handle_type> m_handle;
      std::vector<value_type> m_data;
    };


    template<class Distribution, class Generator,
      int RowsInBlock, int ColsInBlock>
      class random_block_provider
    {
    public:
      using value_type = typename Distribution::result_type;
      using seed_type = typename Generator::result_type;
      using block = data_block<value_type>;
      using seed_and_block = std::pair<seed_type, block>;
     

    public:
      random_block_provider(int rows, int cols, Distribution distribution
        , Generator generator)
        : m_rows(rows), m_cols(cols), m_distribution(distribution), m_lru(g_lru), m_generator(generator)
      {
        int num_block_rows = 1 + (rows - 1) / RowsInBlock;
        int num_block_cols = 1 + (cols - 1) / ColsInBlock;
        int num_blocks = num_block_rows * num_block_cols;
        m_blocks.resize(num_blocks);
        for (auto&& b : m_blocks) {
          b.first = m_generator(); //first: seed
        }
      }
      int rows() const
      {
        return m_rows;
      }

      int cols() const
      {
        return m_cols;
      }

      ~random_block_provider()
      {
        for (auto&& b : m_blocks) {
          if (b.second.has_data()) { // second: block
            m_lru.remove(b.second.get_handle()); // will cause the deletion of any m_data
          }
        }
      }
      
      int block_rows() const
      {
        return RowsInBlock;
      }
      
      int block_cols() const
      {
        return ColsInBlock;
      }
      
      void add_lock(block* b)
      {
        m_lru.add_lock(b->get_handle());
      }

      void drop_lock(block* b)
      {
        m_lru.drop_lock(b->get_handle());
      }
      
      block* get_block(int index) 
      {
        seed_and_block& b = m_blocks[index];
        if (b.second.has_data()) {
          m_lru.touch(b.second.get_handle());
        }
        else {
          auto closer = [&b]() { b.second.clear(); } ;
          std::size_t n = RowsInBlock * ColsInBlock;
          std::size_t block_size = sizeof(value_type) * n;

          lru::id handle = m_lru.add(block_size, closer);
          b.second.set_handle_and_resize(handle, n);

          Generator rng(b.first); //first: seed
          for (auto&& j : b.second) { //second: block
            j = m_distribution(rng);
          }
        }
        return &(b.second);
      }

      lru& m_lru = g_lru;
      Distribution m_distribution;
      Generator m_generator;
      std::vector<seed_and_block> m_blocks;
      int m_rows;
      int m_cols;
    };
    
    template<class BlockProvider>
      class cached_data_blocks
    {
    public:
      using value_type = typename BlockProvider::value_type;
      
      int block_rows() const
      {
        return m_block_provider->block_rows();
      }
      int block_cols() const
      {
        return m_block_provider->block_cols();
      }

    public:

      class iterator
      {
        using view_type = cached_data_blocks;
        using block_type = typename BlockProvider::block;
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
          return a.m_pos == b.m_pos;
        }

        friend inline bool operator!=(const iterator& a
          , const iterator& b)
        {
          return !(a==b);
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

          int blocks_per_row = 1 + (m_view->m_full_cols-1) / block_cols;

          int major_index = m_major_row * blocks_per_row + m_major_col;

          int row_in_block = full_row % block_rows;
          int col_in_block = full_col % block_cols;

          int index_in_block = row_in_block * block_cols + col_in_block;
          auto provider = m_view->m_block_provider;
          auto lock_dropper = [provider](block_type* b) {
            provider->drop_lock(b);
          };

          m_block.reset(m_view->m_block_provider->get_block(major_index), lock_dropper);
          m_view->m_block_provider->add_lock(&*m_block);
          
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

      cached_data_blocks(std::shared_ptr<BlockProvider> bp) 
        : m_block_provider(bp), m_rows(bp->rows()), m_cols(bp->cols())
        , m_full_cols(bp->cols()), m_full_rows(bp->rows())
      { }

      cached_data_blocks() = default;
      cached_data_blocks(const cached_data_blocks&) = default;
      cached_data_blocks(cached_data_blocks&&) = default;
      cached_data_blocks& operator=(const cached_data_blocks&) = default;
      cached_data_blocks& operator=(cached_data_blocks&&) = default;

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
	  
      cached_data_blocks sub_raster(int first_row, int first_col, int rows, int cols) const
      {
        cached_data_blocks copy = *this;
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
      std::shared_ptr<BlockProvider> m_block_provider;
      int m_rows; // only the subset rows
      int m_cols; // only the subset rows
      int m_first_row; // first row in subset
      int m_first_col; // first col in subset
      int m_full_rows; // full number of rows, before taking subset
      int m_full_cols; // full number of rows, before taking subset
    };


    // Default block size is 128 x 128
    // Default generator is mersenne twister, seeded by time
    template<int BlockRows = 128, int BlockCols = 128, class Distribution, class Generator = std::mt19937_64>
    cached_data_blocks< random_block_provider<Distribution, Generator, BlockRows, BlockCols > >
      random_distribution_raster(int rows, int cols
        , Distribution dist
        , Generator gen = Generator(std::random_device()()))
    {
      using provider = random_block_provider<Distribution, Generator,
        BlockRows, BlockCols >;
      std::shared_ptr<provider> rbp(new provider(rows, cols, dist, gen));
      return cached_data_blocks<provider>(rbp);
    }
  }
}
