---
layout: single
sidebar: 
  nav: docs
---
# Installation and configuration
The Pronto Raster library is a C++ library that depends on C+\+20 language features and STL and the GDAL library.

See the websites of [GDAL](http://www.gdal.org) for guidance on installing that library. If you are using Microsoft Visual Studio it can be useful to use [vcpkg](https://github.com/Microsoft/vcpkg). 

In order to invoke the tests, there is a further dependency on [Google Test](https://github.com/google/googletest).

In order to run the benchmarks, there is a further dependency on [Google Benchmark](https://github.com/google/benchmark). Some of the benchmarks require Python and the Python numpy library, This is just for counter-benchmarks, i.e. to compare against other solutions than Pronto Raster.


Some functions in the library require the compilation of a large number of functions. In particular this is for functions on the any_blind_raster class, that are compiled for all (combinations of) supported datatypes. In Visual Studio the large number of functions requires the [/bigobj setting](https://msdn.microsoft.com/en-us/library/ms173499.aspx).

New developments are tested on a recent version of Visual Studio only.
