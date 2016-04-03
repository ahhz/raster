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
      dereference_proxy(const PutGetIterator* iter) :m_iter(iter)
      {}

      // conversion to make the iterator readable
      operator ValueType() const
      {
        return m_iter->get();
      }

      // assignment to make the iterator writable
      const dereference_proxy& operator=(const ValueType& v) const
      {
        m_iter->put(v);
        return *this;
      }

      template<class T, class I>
      const dereference_proxy& operator=(const dereference_proxy<I, T>& that) const
      {
        return operator=(static_cast<T>(that));
      }

      const dereference_proxy& operator=(const dereference_proxy& that) const
      {
        return operator=(static_cast<ValueType>(that));
      }

       const dereference_proxy& operator++() const
      {
        m_iter->put(++m_iter->get());
        return *this;
      }
      const dereference_proxy& operator--() const
      {
        m_iter->put(--m_iter->get());
        return *this;
      }
      
      ValueType operator++(int) const
      {
        ValueType temp = m_iter->get();
        m_iter->put(++m_iter->get());
        return temp;
      }
      ValueType operator--(int) const
      {
        ValueType temp = m_iter->get();
        m_iter->put(--m_iter->get());
        return temp;
      }
      
      template<class T>
      const dereference_proxy& operator+=(const T& v) const
      {
        m_iter->put(m_iter->get() + v);
        return *this;
      }

      template<class T>
      const dereference_proxy& operator-=(const T& v) const
      {
        m_iter->put(m_iter->get() - v);
        return *this;
      }
      
      template<class T>
      const dereference_proxy& operator/=(const T& v) const
      {
        m_iter->put(m_iter->get() / v);
        return *this;
      }

      template<class T>
      const dereference_proxy& operator*=(const T& v) const
      {
        m_iter->put(m_iter->get() * v);
        return *this;
      }

      template<class T>
      const dereference_proxy& operator%=(const T& v) const
      {
        m_iter->put(m_iter->get() % v);
        return *this;
      }

      template<class T>
      const dereference_proxy& operator&=(const T& v) const
      {
        m_iter->put(m_iter->get() & v);
        return *this;
      }
      
      template<class T>
      const dereference_proxy& operator|=(const T& v) const
      {
        m_iter->put(m_iter->get() | v);
        return *this;
      }

      template<class T>
      const dereference_proxy& operator^=(const T& v) const
      {
        m_iter->put(m_iter->get() ^ v);
        return *this;
      }

      template<class T>
      const dereference_proxy& operator<<=(const T& v) const
      {
        m_iter->put(m_iter->get() << v);
        return *this;
      }

      template<class T>
      const dereference_proxy& operator>>=(const T& v) const
      {
        m_iter->put(m_iter->get() >> v);
        return *this;
      }

   // private:
 
      const PutGetIterator* m_iter;
    };
  }
}
#endif //DEREFERENCE_PROXY_H_AHZ