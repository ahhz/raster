//
//=======================================================================
// Copyright 2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// A convenience function, plot the contents of a raster to cout, useful for 
// debugging and small examples. 

#pragma once

#include <pronto/raster/access_type.h>
#include <pronto/raster/raster.h>
#include <pronto/raster/raster_variant.h>


#include <iostream>
#include <optional>

namespace pronto {
  namespace raster {

    // a function object that can apply on any_raster with any value_type
 
    struct raster_plotter
    {
       void cout_value(const unsigned char& v)
      {
        std::cout << (int)v;
      }

      template<class T>
      void cout_value(const T& v)
      {
         std::cout << v;
      }

      template<class T>
      void cout_value(const std::optional<T>& v)
      {
        if (v) cout_value(*v);
        else std::cout << "-";
      }

      template<class T, class U>
      void cout_value(const std::pair<T, U>& v)
      {
        std::cout << "{";
        cout_value(v.first);
        std::cout << ", ";
        cout_value(v.second);
        std::cout << "}";
      }

      template<class Raster>
      void operator()(const Raster& raster)
      {
        using raster_value_type = typename traits<Raster>::value_type;

        int cols = raster.cols();
        int rows = raster.rows();
        std::cout 
          << "Rows: " << rows 
          << ", Cols: " << cols 
          << ", Value type: " << std::string(typeid(raster_value_type).name())
          << std::endl;

        if (rows * cols > 1000) {
          std::cout << "Too large to plot" << std::endl;
          return;
        }
        
        int col = 0;
        for (auto&& v : raster) {

          cout_value(static_cast<raster_value_type>(v));

          if (++col == cols) {
            std::cout << std::endl;
            col = 0;
          } else{
            std::cout << '\t';
          }
        }
        std::cout << std::endl;

      }
    };

    template<class Raster>
    void plot_raster(const Raster& raster)
    {
      raster_plotter{}(raster);
    }

    template<RasterVariantConcept Ras>
    void plot_raster(const Ras& raster)
    {
      std::visit(raster_plotter{}, raster.m_raster);
    }
  }
}