---
layout: single
sidebar: 
  nav: docs
---
# Installation and configuration
The Pronto Raster library is a header-only library that depends on C+\+11 language features and STL and the GDAL library. Depending on the project configuration it further requires the Boost library and/or some C+\+17 features. 
The library can be configured to use the `any`, `filesystem::path` and `optional` classes from either C+\+17 STL or Boost. By default it uses the STL. If you wish to use Boost instead, include the following in your project before including any file from the Pronto Raster library:

```cpp
#define PRONTO_RASTER_USE_BOOST_OPTIONAL
#define PRONTO_RASTER_USE_BOOST_ANY
#define PRONTO_RASTER_USE_BOOST_FILESYSTEM
```
Apart from these three dependencies, there are no other Boost dependencies, so if you have C+\+17 support you do not require Boost.

See the websites of [GDAL](http://www.gdal.org) and [Boost](http://www.boost.org) for guidance on installing these libraries. If you are using Microsoft Visual Studio it can be useful to use [vcpkg](https://github.com/Microsoft/vcpkg). It automatically takes care of any further dependencies of both Boost and GDAL and makes it straightforward to built separately for 32/64 bit and Debug/Release.

In order to invoke the tests, there is a further dependency on [Google Test](https://github.com/google/googletest).

In order to run the benchmarks, there is a further dependency on [Google Benchmark](https://github.com/google/benchmark). Some of the benchmarks require Python and the Python numpy library, This is just for counter-benchmarks, i.e. to compare against other solutions than Pronto Raster.


Some functions in the library require the compilation of a large number of functions. In particular this is for functions on the any_blind_raster class, that are compiled for all (combinations of) supported datatypes. In Visual Studio the large number of functions requires the [/bigobj setting](https://msdn.microsoft.com/en-us/library/ms173499.aspx).

New developments are tested no a recent version of Visual Studio and GCC only.
