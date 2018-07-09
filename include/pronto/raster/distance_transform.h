//
//=======================================================================
// Copyright 2016-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// This implements the distance transform method by Meijster. 
// The method is very amenable to parallelization. 
// But not done here..

// TODO: Read this https://stackoverflow.com/questions/43655668/are-all-integer-values-perfectly-represented-as-doubles
// and make a sensible check for the type of the output raster, knowing that we also use it to store ints.

#pragma once
#include <pronto/raster/assign.h>
#include <pronto/raster/traits.h>
#include <pronto/raster/transform_raster_view.h>

#include <cassert>
#include <algorithm>
#include <vector>

 namespace pronto {
   namespace raster {
    namespace detail
    {
      struct euclidean{};
    }
    struct euclidean_squared : public detail::euclidean{};
    struct euclidean_non_squared : public detail::euclidean{};
    struct manhattan{};
    struct chessboard{};

    namespace detail
    {
       int round(const double& f)
      {
        return static_cast<int>(f + 0.5);
      }

      template<class T>
      double optionally_square_root(const T& value, 
        const euclidean_non_squared&)
      {
        return sqrt(static_cast<double>(value));
      }

      template<class T, class OtherMethod>
      T optionally_square_root(const T& value, const OtherMethod&)
      {
        return value;
      }
      int f(int x, int i, std::vector<int>& g, const detail::euclidean&)
      {
        const int dx = x - i;
        const int dy = g[i];
        return dx * dx + dy * dy;
      }

      int f(int x, int i, std::vector<int>& g, const manhattan&)
      {
        return abs(x - i) + g[i];
      }

      int f(int x, int i, std::vector<int>& g, const chessboard&)
      {
        return std::max(abs(x - i), g[i]);
      }

      int sep(int i, int u, std::vector<int>& g, int, const detail::euclidean&)
      {
        return ( (u-i) * (u+i) + (g[u] - g[i]) * (g[u] + g[i] ) ) / (2 * (u - i));
      }

      int sep(int i, int u, std::vector<int>& g, int inf, const manhattan&)
      {
        if (g[u] >= g[i] + u - i) return inf;
        if (g[i] > g[u] + u - i) return -inf;
        return (g[u] - g[i] + u + i) / 2;
      }

      int sep(int i, int u, std::vector<int>& g, int, const chessboard&)
      {
        if (g[i] <= g[u]) return std::max(i + g[u], (i + u) / 2);
        return std::min(u - g[i], (i + u) / 2);
      }

      struct st_pair
      {
        st_pair(int s, int t) : s(s), t(t)
        {}
        int s;
        int t;
      };

      template<class ResultRow, class MethodTag>
      bool process_line(ResultRow& result_row, int inf, 
        const MethodTag&)
      {
        int cols = result_row.cols();
        std::vector<int> g(cols);
        auto g_i = std::rbegin(g);
        auto r_i = std::begin(result_row);

        // g has the values of the result row in reverse order
        for (; g_i != std::rend(g); ++g_i, ++r_i) {
          // the result_row may be doubles to  hold sqrt-ed distances
          *g_i = static_cast<int>(*r_i);
        }
        const int m = static_cast<int>(g.size());
        std::vector<st_pair> st(1, st_pair(0, 0));
        for (int u = 1; u < m; ++u) {
          while (!st.empty() && f(st.back().t, st.back().s, g, MethodTag{})
                > f(st.back().t, u, g, MethodTag{})){
            st.pop_back();
          }
          if (st.empty()){
            st.emplace_back(u, 0);
          }
          else {
            const int w = 1 + sep(st.back().s, u, g, inf, MethodTag{});
            if (w < m){
              st.emplace_back(u, w);
            }
          }
        }
        auto iter = result_row.begin();
        bool has_target = f(m-1, st.back().s, g, MethodTag{}) != inf;
        for (int u = m - 1; u >= 0; --u, ++iter) {
          //++iter because g was in reverse
          *iter = optionally_square_root(f(u, st.back().s, g, MethodTag{}),
            MethodTag{});
          if (u == st.back().t) {
            st.pop_back();
          }
        }
        return has_target;
      }
    }

    // Return false if target is not present in raster, true otherwise
    template<class InRaster, class OutRaster>
    bool euclidean_distance_transform(const InRaster& in, OutRaster& out,
      const typename traits<InRaster>::value_type& target)
    {
      return distance_transform(in, out, target, euclidean_non_squared{});
    }

    // Return false if target is not present in raster, true otherwise
    template<class InRaster, class OutRaster>
    bool squared_euclidean_distance_transform(const InRaster& in, OutRaster& out
      , const typename traits<InRaster>::value_type& target)
    {
      return distance_transform(in, out, target, euclidean_squared{});
    }

    // Return false if target is not present in raster, true otherwise
    template<class InRaster, class OutRaster>
    bool manhattan_distance_transform(const InRaster& in, OutRaster& out,
      const typename traits<InRaster>::value_type& target)
    {
      return distance_transform(in, out, target, manhattan{});
    }

    // Return false if target is not present in raster, true otherwise
    template<class InRaster, class OutRaster>
    bool chessboard_distance_transform(const InRaster& in, OutRaster& out,
      const typename traits<InRaster>::value_type& target)
    {
      return distance_transform(in, out, target, chessboard{});
    }

    // Return false if target is not present in raster, true otherwise
    template<class InRaster, class OutRaster, class Method>
    bool distance_transform(const InRaster& in, OutRaster& out,
      const typename traits<InRaster>::value_type& target,
      const Method&)
    {
      using in_type = typename traits<InRaster>::value_type;
      using out_type = typename traits<OutRaster>::value_type;
      const int rows = in.rows();
      const int cols = in.cols();
      assert(rows == out.rows());
      assert(cols == out.cols());
      const int inf = rows + cols;

      auto a = in.begin();
      auto v = out.begin();

      // Going by rows instead of by columns as in the paper. 

      int r = 0; // for (int r = 0; r < 1; ++r) {
      
      auto in_row = in.sub_raster(r, 0, 1, cols);
      auto out_row = out.sub_raster(r, 0, 1, cols);
      auto down_first = [&inf, &target](const in_type& a)
      {
        if (a == target) return 0;
        else return inf;
      };

      assign(out_row, transform(down_first, in_row));
      
      for (int r = 1; r < rows; ++r) {
        auto in_row = in.sub_raster(r, 0, 1, cols);
        auto out_row = out.sub_raster(r, 0, 1, cols);
        auto above_row = out.sub_raster(r-1, 0, 1, cols);

        auto down = [&inf, &target](const in_type& a, const out_type& above)
        {
          if (a == target) return 0;
          else if (above == inf) return inf;
          else return static_cast<int>(above) + 1;
        };

        assign(out_row, transform(down, in_row, above_row));
      }

      for (int r = rows - 2; r >= 0; --r) {
        auto out_row = out.sub_raster(r, 0, 1, cols);
        auto below_row = out.sub_raster(r+1, 0, 1, cols);

        auto up = [](const out_type& a, const out_type& below)
        {
          return std::min(a, below + 1);
        };

        assign(out_row, transform(up, out_row, below_row));

        detail::process_line(below_row, inf, Method{});
      }

      auto first_row = out.sub_raster(0, 0, 1, cols);
      bool has_target = detail::process_line(first_row, inf, Method{});
      return has_target;
    }
  }
}
