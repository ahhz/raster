//
//=======================================================================
// Copyright 2015
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// The class dereference_proxy is used with proxied iterators that 
// have get() and put() accessors. 
//
// Dereferencing a proxied iterator will return a object of this class.
// It can be cast into the value_type of the iterator, and it can also 
// be assigned a value_type value
//
// The intended use is
//
//	 value_type v = *iterator; 
//   equivalent to: value_type v = iterator.get()
//
// or:
//
//	 *iterator = v;
//   equivalent to: iterator.put(v);
//
// but:
//   value_type& v = *iterator; // won't work
//   iterator->value_member = vm; // won't work

#ifndef BLINK_RASTER_DEREFERENCE_PROXY_H_AHZ
#define BLINK_RASTER_DEREFERENCE_PROXY_H_AHZ

namespace blink {
  namespace raster {
    // an iterator with put(v) and get() member function
    template<typename PutGetIterator, typename ValueType>
    struct dereference_proxy
    {
      // typedef typename PutGetIterator::value_type value_type;
      // Cannot get the valuetype from the iterator, because it may not be complete

      // assignment to make the iterator writable
      void operator=(const ValueType& v) const
      {
        m_iter->put(v);
      }

      void operator=(const dereference_proxy& that) const
      {
        m_iter->put(that.m_iter->get());
      }


      // conversion to make the iterator readable
      operator ValueType() const
      {
        return m_iter->get();
      }

   // private:
      dereference_proxy(const PutGetIterator* iter) :m_iter(iter)
      {}

      const PutGetIterator* m_iter;
    };
  }
}
#endif //DEREFERENCE_PROXY_H_AHZ