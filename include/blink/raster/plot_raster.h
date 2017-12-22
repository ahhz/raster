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

#include <blink/raster/any_blind_raster.h>
#include <blink/raster/blind_function.h>
#include <blink/raster/optional.h>
#include <blink/raster/raster_algebra_wrapper.h>

#include <iostream>

namespace blink {
  namespace raster {

    // a function object that can apply on any_raster with any value_type
    struct value_type_getter
    {
      template<class T>
      std::string  operator()(const any_raster<T>& raster) const
      {
        return std::string(typeid(T).name());
      }
    };

    template<class Raster>
    std::string get_value_type(const Raster& raster)
    {
      using value_type = typename traits<Raster>::value_type;
      return std::string(typeid(value_type).name());

    }
    std::string get_value_type(const any_blind_raster& raster)
    {
      return blind_function(value_type_getter{}, raster);
    }

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
      void cout_value(const optional<T>& v)
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
          << ", Value type: " << get_value_type(raster) 
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

    void plot_raster(any_blind_raster raster)
    {
      blind_function(raster_plotter{}, raster);
    }

    void plot_raster(raster_algebra_wrapper<any_blind_raster> raster)
    {
      blind_function(raster_plotter{}, raster.unwrap());
    }
  }
}