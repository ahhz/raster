//=======================================================================
// Copyright 2015-2107
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
// 

#pragma once
#include <pronto/raster/cached_block_raster_view.h>
#include <pronto/raster/lru.h>

#include <memory>
#include <random> 
#include <utility> 
#include <vector> 

namespace pronto
{
  namespace raster
  {
    template<int RowsInBlock, int ColsInBlock, 
      class Distribution, class Generator>
    class random_block_provider
    {
    public:
      using value_type = typename Distribution::result_type;
      using seed_type = typename Generator::result_type;
      using block = data_block<value_type>;
      using seed_and_block = std::pair<seed_type, block>;

      random_block_provider(int rows, int cols, Distribution distribution
        , Generator generator)
        : m_rows(rows), m_cols(cols), m_distribution(distribution)
        , m_generator(generator)
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
              
      int block_rows() const
      {
        return RowsInBlock;
      }
      
      int block_cols() const
      {
        return ColsInBlock;
      }
      
      block* get_block(int index) 
      {
        seed_and_block& b = m_blocks[index];

        if (b.second.has_data()) {

          b.second.touch();
        }
        else {
         
          b.second.create_data(RowsInBlock * ColsInBlock);

          Generator rng(b.first);   //first: seed
          for (auto&& j : b.second) //second: data
          {
            j = m_distribution(rng);
          }
        }
        return &(b.second);
      }
    private: 
      Distribution m_distribution;
      Generator m_generator;
      std::vector<seed_and_block> m_blocks;
      int m_rows;
      int m_cols;
    };

    template<int BlockRows, int BlockCols, class Distribution, class Generator,
      bool IsForwardOnly>
    using random_raster_view = cached_block_raster_view<
      random_block_provider<BlockRows, BlockCols, Distribution, Generator>
      , false // not mutable
      , IsForwardOnly 
    >;
    
    // Default block size is 128 x 128
    // Default generator is mersenne twister, seeded by random_device
    // Default is not forward-only
    template<
      int BlockRows = 128, 
      int BlockCols = 128, 
      class Distribution, 
      class Generator = std::mt19937_64, 
      bool IsForwardOnly = false
    >
    random_raster_view<
      BlockRows, 
      BlockCols, 
      Distribution, 
      Generator, 
      IsForwardOnly
    >
      random_distribution_raster(
        int rows, 
        int cols, 
        Distribution dist, 
        Generator gen = Generator(std::random_device()()))
    {
      using provider = random_block_provider<BlockRows, BlockCols, 
        Distribution, Generator>;
      std::shared_ptr<provider> rbp(new provider(rows, cols, dist, gen));
      return cached_block_raster_view<provider, false, IsForwardOnly>(rbp);
    }
  }
}
