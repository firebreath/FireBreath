/*
 * (C) Copyright Christopher Diggins 2005
 * (C) Copyright Pablo Aguilar 2005
 * (C) Copyright Kevlin Henney 2001
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)  
 */

#ifndef CDIGGINS_ANY_HPP
#define CDIGGINS_ANY_HPP

#define ANY_IMPLICIT_CASTING    // added to enable implicit casting

#include <stdexcept>
#include <typeinfo>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <string>

#ifdef _WIN32
#pragma warning( disable : 4800 )
#endif

#define BEGIN_CONVERT_MAP(_type_) \
    const type_info *type = &get_type(); \
    if (*type == typeid(_type_)) { \
        return cast<_type_>(); \
    } else

#define END_CONVERT_MAP(_type_) { throw bad_any_cast(get_type(), typeid(_type_)); }

#define CONVERT_ENTRY_SIMPLE(_type_, _srctype_)             \
    if ( *type == typeid( _srctype_ ) ) {              \
        return static_cast< _type_ >( cast< _srctype_ >() );\
    } else

#define CONVERT_ENTRY_COMPLEX_BEGIN(_srctype_, _var_) \
    if (*type == typeid(_srctype_)) { \
        _srctype_ _var_ = cast<_srctype_>();

#define CONVERT_ENTRY_COMPLEX_END() \
    } else

#define CONVERT_ENTRY_TOSTRING(_srctype_)             \
    CONVERT_ENTRY_COMPLEX_BEGIN(_srctype_, __varname) \
    std::stringstream sstr;                           \
    sstr << __varname;                                \
    return sstr.str();                                \
    CONVERT_ENTRY_COMPLEX_END()

namespace cdiggins
{
  struct bad_any_cast : std::bad_cast {
    bad_any_cast(const std::type_info& src, const std::type_info& dest)
      : from(src.name()), to(dest.name())
    { }
    virtual const char* what() { 
      return "bad cast"; 
    }
    const char* from;
    const char* to;
  };

  namespace any_detail
  {
    // function pointer table
    struct fxn_ptr_table {
      const std::type_info& (*get_type)();
      void (*static_delete)(void**);
      void (*clone)(void* const*, void**);
      void (*move)(void* const*,void**);
    };

      // static functions for small value-types 
      template<bool is_small>
      struct fxns
      {
          template<typename T>
          struct type {
            static const std::type_info& get_type() { 
              return typeid(T); 
            }
            static void static_delete(void** x) { 
              reinterpret_cast<T*>(x)->~T(); 
            }
            static void clone(void* const* src, void** dest) { 
              new(dest) T(*reinterpret_cast<T const*>(src)); 
            }
            static void move(void* const* src, void** dest) { 
              reinterpret_cast<T*>(dest)->~T(); 
              *reinterpret_cast<T*>(dest) = *reinterpret_cast<T const*>(src); 
             }
          };
      };

    // static functions for big value-types (bigger than a void*)
    template<>
      struct fxns<false>
      {
          template<typename T>
          struct type {
            static const std::type_info& get_type() { 
              return typeid(T); 
            }
            static void static_delete(void** x) { 
              delete(*reinterpret_cast<T**>(x)); 
            }
            static void clone(void* const* src, void** dest) { 
              *dest = new T(**reinterpret_cast<T* const*>(src)); 
            }
            static void move(void* const* src, void** dest) { 
              (*reinterpret_cast<T**>(dest))->~T(); 
              **reinterpret_cast<T**>(dest) = **reinterpret_cast<T* const*>(src); 
            }
          };
      };

    template<typename T>
    struct get_table 
    {
      static const bool is_small = sizeof(T) <= sizeof(void*);
      
        static fxn_ptr_table* get()
        {
            static fxn_ptr_table static_table = {
                fxns<is_small>::template type<T>::get_type
              , fxns<is_small>::template type<T>::static_delete
              , fxns<is_small>::template type<T>::clone
              , fxns<is_small>::template type<T>::move
            };
            return &static_table;
        }
    };
    
    struct empty {
    };
  } // namespace any_detail

  struct any
  {
    // structors
    template <typename T>
    any(const T& x) {
      table = any_detail::get_table<T>::get();
      if (sizeof(T) <= sizeof(void*)) {
        new(&object) T(x);
      }
      else {        
        object = new T(x);
      }
    }

    any(const char *x) {
        table = any_detail::get_table<any_detail::empty>::get();
        object = NULL;
        assign(x);
    }

    any() {
        table = any_detail::get_table<any_detail::empty>::get();
        object = NULL;
    }

    // Utilize the assignment operator
    any(const any& x) {
        *this = x;
    }
    
    ~any() {
        table->static_delete(&object);
    }

    // assignment
    any& assign(const any& x) {
      // are we copying between the same type?
      if (table == x.table) {
        // if so, we can avoid reallocation
        table->move(&x.object, &object);
      }
      else {
        reset();
        x.table->clone(&x.object, &object);				          
        table = x.table;
      }
      return *this;
    }

    any& assign(const char *x) {
        return assign(std::string(x));
    }

    template <typename T>
    any& assign(const T& x)
    {
      // are we copying between the same type?
          any_detail::fxn_ptr_table* x_table = any_detail::get_table<T>::get();
      if (table == x_table) {
        // if so, we can avoid deallocating and resuse memory 
        if (sizeof(T) <= sizeof(void*)) {
          // create copy on-top of object pointer itself
          new(&object) T(x);
        }
        else {
          // create copy on-top of old version
          new(object) T(x);
        }
      }
      else {        
        reset();
        if (sizeof(T) <= sizeof(void*)) {
          // create copy on-top of object pointer itself
          new(&object) T(x);
          // update table pointer 
          table = x_table;
        }
        else {
          object = new T(x);          
          table = x_table;
        }
        }
      return *this;
    }

    // assignment operator 
      template<typename T>
      any& operator=(T const& x) {
          return assign(x);
      }

      any& operator=(any const& rh) {
        table = any_detail::get_table<any_detail::empty>::get();
        return assign(rh);
      }

    // utility functions
    any& swap(any& x) {
      std::swap(table, x.table);
      std::swap(object, x.object);
    }

    const std::type_info& get_type() const {
      return table->get_type();
    }

    template<typename T>
    const T& cast() const {
        if (get_type() != typeid(T)) {
          throw bad_any_cast(get_type(), typeid(T));
        }
        if (sizeof(T) <= sizeof(void*)) {
        return *reinterpret_cast<T const*>(&object);
        }
        else {
        return *reinterpret_cast<T const*>(object);
        }
    }

    template<typename T>
    const T convert_cast() const {
        return cast<T>();
    }

    template<> const int convert_cast<int>() const {
        BEGIN_CONVERT_MAP(int)
        CONVERT_ENTRY_SIMPLE(int, double)
        CONVERT_ENTRY_SIMPLE(int, float)
        CONVERT_ENTRY_SIMPLE(int, char)
        CONVERT_ENTRY_SIMPLE(int, unsigned char)
        CONVERT_ENTRY_SIMPLE(int, short)
        CONVERT_ENTRY_SIMPLE(int, unsigned short)
        CONVERT_ENTRY_COMPLEX_BEGIN(std::string, str)
            return strtol(str.c_str(), NULL, 10);
        CONVERT_ENTRY_COMPLEX_END()
        END_CONVERT_MAP(int)
    }

    template<> const double convert_cast<double>() const {
        BEGIN_CONVERT_MAP(double);
        CONVERT_ENTRY_SIMPLE(double, float);
        CONVERT_ENTRY_SIMPLE(double, char);
        CONVERT_ENTRY_SIMPLE(double, unsigned char);
        CONVERT_ENTRY_SIMPLE(double, short);
        CONVERT_ENTRY_SIMPLE(double, unsigned short);
        CONVERT_ENTRY_SIMPLE(double, int);
        CONVERT_ENTRY_SIMPLE(double, unsigned int);
        CONVERT_ENTRY_SIMPLE(double, long);
        CONVERT_ENTRY_SIMPLE(double, unsigned long);
        CONVERT_ENTRY_COMPLEX_BEGIN(std::string, str)
            return strtod(str.c_str(), NULL);
        CONVERT_ENTRY_COMPLEX_END()
        END_CONVERT_MAP(double);
    }

    template<> const float convert_cast<float>() const {
        BEGIN_CONVERT_MAP(float);
        CONVERT_ENTRY_SIMPLE(float, char);
        CONVERT_ENTRY_SIMPLE(float, unsigned char);
        CONVERT_ENTRY_SIMPLE(float, short);
        CONVERT_ENTRY_SIMPLE(float, unsigned short);
        CONVERT_ENTRY_SIMPLE(float, int);
        CONVERT_ENTRY_SIMPLE(float, unsigned int);
        CONVERT_ENTRY_COMPLEX_BEGIN(std::string, str)
            return static_cast<float>(strtod(str.c_str(), NULL));
        CONVERT_ENTRY_COMPLEX_END()
        END_CONVERT_MAP(double);
    }

    template<> const long convert_cast<long>() const {
        BEGIN_CONVERT_MAP(long);
        CONVERT_ENTRY_SIMPLE(long, double);
        CONVERT_ENTRY_SIMPLE(long, float);
        CONVERT_ENTRY_SIMPLE(long, char);
        CONVERT_ENTRY_SIMPLE(long, unsigned char);
        CONVERT_ENTRY_SIMPLE(long, short);
        CONVERT_ENTRY_SIMPLE(long, unsigned short);
        CONVERT_ENTRY_SIMPLE(long, int);
        CONVERT_ENTRY_SIMPLE(long, unsigned int);
        CONVERT_ENTRY_COMPLEX_BEGIN(std::string, str)
            return strtol(str.c_str(), NULL, 10);
        CONVERT_ENTRY_COMPLEX_END()
        END_CONVERT_MAP(long);
    }

    template<> const short convert_cast<short>() const {
        BEGIN_CONVERT_MAP(short);
        CONVERT_ENTRY_SIMPLE(short, double);
        CONVERT_ENTRY_SIMPLE(short, float);
        CONVERT_ENTRY_SIMPLE(short, char);
        CONVERT_ENTRY_SIMPLE(short, unsigned char);
        CONVERT_ENTRY_COMPLEX_BEGIN(std::string, str);
            return static_cast<short>(strtol(str.c_str(), NULL, 10));
        CONVERT_ENTRY_COMPLEX_END();
        END_CONVERT_MAP(short);
    }

    template<> const std::string convert_cast<std::string>() const {
        BEGIN_CONVERT_MAP(std::string);
        CONVERT_ENTRY_TOSTRING(double);
        CONVERT_ENTRY_TOSTRING(float);
        CONVERT_ENTRY_TOSTRING(int);
        CONVERT_ENTRY_TOSTRING(unsigned int);
        CONVERT_ENTRY_COMPLEX_BEGIN(bool, bval);
            return bval ? "true" : "false";
        CONVERT_ENTRY_COMPLEX_END();
        CONVERT_ENTRY_TOSTRING(long);
        CONVERT_ENTRY_TOSTRING(unsigned long);
        CONVERT_ENTRY_TOSTRING(short);
        CONVERT_ENTRY_TOSTRING(unsigned short);
        CONVERT_ENTRY_TOSTRING(char);
        CONVERT_ENTRY_TOSTRING(unsigned char);
        END_CONVERT_MAP(std::string);
    }

    template<> const bool convert_cast<bool>() const {
        BEGIN_CONVERT_MAP(bool);
        CONVERT_ENTRY_SIMPLE(bool, double);
        CONVERT_ENTRY_SIMPLE(bool, float);
        CONVERT_ENTRY_SIMPLE(bool, char);
        CONVERT_ENTRY_SIMPLE(bool, unsigned char);
        CONVERT_ENTRY_COMPLEX_BEGIN(std::string, str);
            transform(str.begin(), str.end(), str.begin(), ::tolower); 
            return (str == "y" || str == "1" || str == "yes" || str == "true" || str == "t");
        CONVERT_ENTRY_COMPLEX_END();
        END_CONVERT_MAP(short);
    }

  // implicit casting is disabled by default for compatibility with boost::any 
  #ifdef ANY_IMPLICIT_CASTING
    // automatic casting operator
    template<typename T>
    operator T() const {
      return convert_cast<T>();
    }
  #endif // implicit casting

    bool empty() const {
      //return object == NULL;
      return table == any_detail::get_table<any_detail::empty>::get();
    }

    void reset()
      {
        if (empty()) return; 
      table->static_delete(&object);
        table = any_detail::get_table<any_detail::empty>::get();
      object = NULL;
      }

    // fields
      any_detail::fxn_ptr_table* table;	  
    void* object;
  };

  // boost::any-like casting
    template<typename T>
    T* any_cast(any* this_) {
    if (this_->get_type() != typeid(T)) {
      throw bad_any_cast(this_->get_type(), typeid(T));
    }
      if (sizeof(T) <= sizeof(void*)) {
      return reinterpret_cast<T*>(&this_->object);
      }
      else {
      return reinterpret_cast<T*>(this_->object);
      }
    }

    template<typename T>
    T const* any_cast(any const* this_) {
    return any_cast<T>(const_cast<any*>(this_));
    }

  template<typename T>
  T const& any_cast(any const& this_){
        return *any_cast<T>(const_cast<any*>(&this_));
  }
}

#endif // CDIGGINS_ANY_HPP
