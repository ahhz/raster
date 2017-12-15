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

#ifndef BLINK_RASTER_USE_BOOST_ANY

#include <any>
namespace blink {
  namespace raster {
    using std::any;
    using std::any_cast;
  }
}

#else

#include <boost/any.hpp>
namespace blink {
  namespace raster {
    using boost::any;
    using boost::any_cast;
  }
}

#endif
