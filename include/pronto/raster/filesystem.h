//
//=======================================================================
// Copyright 2016-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#pragma once

#include <filesystem> 
#include <random>
#include <string>

namespace pronto {
  namespace raster {
    namespace filesystem = std::filesystem;
   
    inline filesystem::path get_unique_path(const filesystem::path& path)
    {

      const wchar_t hex[] = L"0123456789abcdef";
      std::random_device rd;  //Will be used to obtain a seed for the random number engine
      std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
      std::uniform_int_distribution<int> dis(0, 15);

      // convert path to wstring
      std::wstring s = path.wstring();

      // replace % for random hex number
      for (auto&& ch : s) {
        if (ch == L'%')  {
          ch = hex[dis(gen)];
        }
      }
      return filesystem::path(s);
    }
  }
}

