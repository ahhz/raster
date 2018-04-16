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
namespace pronto {
  namespace raster {
    using std::any;
    using std::any_cast;
	using std::bad_any_cast;
  }
}

#else

#include <boost/any.hpp>
namespace pronto {
  namespace raster {
    using boost::any;
    using boost::any_cast;
	using boost::bad_any_cast;
  }
}

#endif
