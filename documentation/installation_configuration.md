## Installation and configuration
The Blink Raster library is a header-only library that depends on C+\+11 language features and STL and the GDAL library. Depending on the project configuration it further requires the Boost library and/or some C+\+17 features. 
The library can be configured to use the `any`, `filesystem::path` and `optional` classes from either C+\+17 STL or Boost. By default it uses the STL. If you wish to use  use Boost instead, include the following in your project before including any file from the Blink Raster library:

```cpp
#define BLINK_RASTER_USE_BOOST_OPTIONAL
#define BLINK_RASTER_USE_BOOST_ANY
#define BLINK_RASTER_USE_BOOST_FILESYSTEM
```
Apart from these three dependencies, there are no other Boost dependencies, so if you have C+\+17 support you do not require Boost.

See the websites of [GDAL](http://www.gdal.org) and [Boost](http://www.boost.org) for guidance on installing these libraries. If you are using Microsoft Visual Studio it can be useful to use [vcpkg](https://github.com/Microsoft/vcpkg). It automatically takes care of any further dependencies of both Boost and GDAL and makes it straightforward to built separately for  32/64 bit and Debug/Release.

