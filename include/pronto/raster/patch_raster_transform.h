//
//=======================================================================
// Copyright 2015-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// This file contains the functions for delineating patches in the dataset. 
// For very large datasets this can cause trouble as the algorithm creates a 
// std::deque of cell values that can turn very large.
// Before using a std::deque (FIFO) we used a std::stack (LIFO), std::deque 
// keeps the stack/deque smaller and saves the day for the large CORINE dataset
//

#pragma once

#include <pronto/raster/optional.h>
#include <pronto/raster/io.h>

#include <deque>
#include <memory>
#include <type_traits>  //is_same 
#include <vector>


namespace pronto {
  namespace raster {

    enum class contiguity
    {
      queen,
      rook
    };
    
    using queen_contiguity = std::integral_constant<contiguity,contiguity::queen>;
    using rook_contiguity = std::integral_constant<contiguity, contiguity::rook>;
    
    struct patch_info
    {
      int m_area;
      int m_perimeter;
      int m_category;
    };
  
    struct patch_info_lookup
    {
      patch_info_lookup() = default;
      patch_info_lookup(const patch_info_lookup&) = default;
      patch_info_lookup(patch_info_lookup&&) = default;
      patch_info_lookup& operator=(const patch_info_lookup&) = default;
      patch_info_lookup& operator=(patch_info_lookup&&) = default;

      patch_info_lookup(std::shared_ptr<std::vector<patch_info> > patch_info)
        : m_patch_info(patch_info)
      {}
    
      optional<patch_info> operator()(int index) const
      {
        if(index  != -1){
          return (*m_patch_info)[index];
        } else{
          return none;
        }
      }
  
    private:
      std::shared_ptr<std::vector<patch_info> > m_patch_info;
    };

    template<class Raster, contiguity Contiguity>
    class patch_raster_transform
    {
      using full_patch_info_raster = transform_raster_view<patch_info_lookup, gdal_raster_view<int> >;
      using patch_info_raster = typename traits<full_patch_info_raster>::sub_raster;
 
    public:
      patch_raster_transform() = default;
      patch_raster_transform(const patch_raster_transform&) = default;
      patch_raster_transform(patch_raster_transform&&) = default;
      patch_raster_transform& operator=(patch_raster_transform&&) = default;
      patch_raster_transform& operator=(const patch_raster_transform&) = default;
    
      patch_raster_transform(Raster raster) 
        : m_raster(raster)
        , m_rows(raster.rows())
        , m_cols(raster.cols())
        , m_first_row(0)
        , m_first_col(0)
        , m_index_raster_initialized(false)
        , m_patch_raster_initialized(false)
      {
        create_index_raster();

        m_patch_info = std::make_shared<std::vector<patch_info> >();
        auto full = transform(patch_info_lookup(m_patch_info), m_index);
        m_patch_info_raster = full.sub_raster(m_first_row, m_first_col, m_rows, 
          m_cols);
      }
 
      using iterator = typename traits<patch_info_raster>::const_iterator;
    
      iterator begin() const
      {
        initialize();
        return m_patch_info_raster.begin();
      }
    
      iterator end() const
      {
        return m_patch_info_raster.end();
      }
      int cols() const
      {
        return m_cols;
      }

      int rows() const
      {
        return m_rows;
      }

      patch_raster_transform sub_raster(int first_row, int first_col, 
        int rows, int cols) const
      {
        auto out = patch_raster_transform();
        out.m_raster = m_raster;
        out.m_index = m_index;
        out.m_patch_info = m_patch_info;
        assert(m_index_raster_initialized);
        out.m_index_raster_initialized = m_index_raster_initialized;
        out.m_patch_raster_initialized = m_patch_raster_initialized;
        out.m_patch_info_raster = m_patch_info_raster.sub_raster(first_row, first_col, 
          rows, cols);
        out.m_first_row = m_first_row + first_row;
        out.m_first_col = m_first_col + first_col;
        out.m_rows = rows;
        out.m_cols = cols;
        return out;
      }

    private:
      using coordinate = std::pair<int, int>;

      static coordinate add(const coordinate& a, const coordinate& b ) 
      {
          return coordinate(a.first + b.first, a.second + b.second);
      }

      // returns the index in the full raster, not subraster
      int index(const coordinate& c) const
      {
        return c.first * m_raster.cols() + c.second; 
      }

      void create_index_raster()
      {
        m_index = create_temp<int>(m_raster.rows(), m_raster.cols());

        if (!m_index_raster_initialized) {
          m_index_raster_initialized = true;
          int nodata = -1;
          for (auto&& i : m_index) {
            i = nodata;
          }
        }
      }
      void initialize() const
      {
        if (m_patch_raster_initialized) return;

        m_patch_raster_initialized = true;
        int nodata = -1;
        std::deque<coordinate> cell_stack;

        static const coordinate N(-1, 0);
        static const coordinate S(1, 0);
        static const coordinate E(0, 1);
        static const coordinate W(0, -1);
        static const coordinate NW(-1, -1);
        static const coordinate NE(-1, 1);
        static const coordinate SW(1, -1);
        static const coordinate SE(1, 1);

        auto index_begin = m_index.begin();
        auto raster_begin = m_raster.begin();
        for (int i = m_first_row; i < m_first_row + m_rows; i++) {
          for (int j = m_first_col; j < m_first_col + m_cols; j++) {
            const coordinate coord(i, j);
            auto index_coord = index(coord);
            if (index_begin[index_coord] == nodata) {

              int this_val = raster_begin[index_coord];
              int this_area = 1;
              int this_perim = 0;
              int this_index = static_cast<int>(m_patch_info->size());

              index_begin[index_coord] = static_cast<int>(m_patch_info->size());
              cell_stack.push_back(coord);

              coordinate nb;

              auto rook_lambda = [&]()
              {
                auto index_nb = index(nb);
                if (raster_begin[index_nb] != this_val) {
                  ++this_perim;
                }
                else if (index_begin[index_nb] == nodata) {
                  cell_stack.push_back(nb);
                  ++this_area;
                  index_begin[index_nb] = this_index;
                }
              };

              auto queen_lambda = [&]()
              {
                auto index_nb = index(nb);
                if (raster_begin[index_nb] == this_val
                  && index_begin[index_nb] == nodata) {
                  cell_stack.push_back(nb);
                  ++this_area;
                  index_begin[index_nb] = this_index;
                }
              };

              while (!cell_stack.empty()) {
                coordinate curr = cell_stack.front();
                cell_stack.pop_front();

                nb = add(curr, N); if (nb.first >= 0) rook_lambda();
                nb = add(curr, S); if (nb.first <= m_raster.rows() - 1) 
                  rook_lambda();
                nb = add(curr, W); if (nb.second >= 0) rook_lambda();
                nb = add(curr, E); if (nb.second <= m_raster.cols() - 1) 
                  rook_lambda();

                // compile time IF
                if (Contiguity == contiguity::queen) {
                  nb = add(curr, NW); if (nb.first >= 0 && nb.second >= 0) 
                    queen_lambda();
                  nb = add(curr, NE); if (nb.first >= 0 && nb.second <= 
                    m_raster.cols() - 1) queen_lambda();
                  nb = add(curr, SW); if (nb.first <= m_raster.rows() - 1 && 
                    nb.second >= 0) queen_lambda();
                  nb = add(curr, SE); if (nb.first <= m_raster.rows() - 1 && 
                    nb.second <= m_raster.cols() - 1) queen_lambda();
                }
              } // while

              patch_info patch;
              patch.m_area = this_area;
              patch.m_perimeter = this_perim;;
              patch.m_category = this_val;

              m_patch_info->push_back(patch);
            } // if
          } // j
        } //i
      }
    
      Raster m_raster;
      mutable gdal_raster_view<int> m_index; 

      // must be declared before m_patch_info_raster
      mutable std::shared_ptr< std::vector<patch_info> > m_patch_info; 
      mutable bool m_index_raster_initialized;
      mutable bool m_patch_raster_initialized;
      mutable patch_info_raster m_patch_info_raster;
      int m_first_row;
      int m_first_col;
      int m_rows;
      int m_cols;
    };

    template<class Raster, class Contiguity>
    patch_raster_transform<Raster, Contiguity::value> patch_raster(Raster r, Contiguity)
    {
          return patch_raster_transform<Raster, Contiguity::value>(r);
    }
  }  
}
