/*
 *  cf_scoped_ptr.h
 *  fbplugin
 *
 *  Like boost::scoped_ptr, but for automatically releasing CoreFoundation types.
 */

#pragma once
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#include <boost/noncopyable.hpp>

inline void release_cf_type(CGImageRef img) { CGImageRelease(img); }
inline void release_cf_type(CFTypeRef obj) { CFRelease(obj); }
inline void release_cf_type(CGContextRef ctx) { CGContextRelease(ctx); }

// CFTypeRef and CGImageRef and the like are all pointer typedefs,
// so unlike scoped_ptr, this doesn't implicitly add a "*" to the
// passed type.

template <typename T> class cf_scoped_ptr : boost::noncopyable {
public:
  cf_scoped_ptr(T _px) : px(_px) {}
  cf_scoped_ptr() : px(NULL) {}
  
  ~cf_scoped_ptr() { 
    reset(NULL);
  }
  
  void reset(T new_ptr = NULL) {
    if (px) release_cf_type(px);
    px = new_ptr;
  }
  
  operator T() const { return px; }
  const T* operator&() const { return &px; }
  T* operator&() { return &px; }  
  bool operator!() const { return !px; }
  operator bool() const { return px; }
  
protected:
  T px;
};

