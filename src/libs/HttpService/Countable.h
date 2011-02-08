/*
 *  Countable.h
 *  fbplugin
 *
 */

#pragma once
#include <cstddef>
#include <cassert>
#include <boost/intrusive_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/detail/atomic_count.hpp>

class Countable : public boost::noncopyable {
public:
  typedef boost::intrusive_ptr<Countable> ptr;
  
  Countable() : _refcount(0) {}
  virtual ~Countable() {
    assert(_refcount == 0);
  }
  
private:
  friend void intrusive_ptr_add_ref(Countable*);
  friend void intrusive_ptr_release(Countable*);
  mutable boost::detail::atomic_count _refcount;
};

inline void intrusive_ptr_add_ref(Countable* obj) {
  ++(obj->_refcount);
}

inline void intrusive_ptr_release(Countable* obj) {
  if ((--(obj->_refcount)) == 0) delete obj;
}

