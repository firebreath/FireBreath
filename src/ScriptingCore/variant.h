/*
* (C) Copyright Christopher Diggins 2005
* (C) Copyright Pablo Aguilar 2005
* (C) Copyright Kevlin Henney 2001
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)  
*
* This file was originally written by Christopher Diggins, and then was modified in 2008
* by Richard Bateman for the FireBreath open source project; it was renamed to FB::variant at that
* time.
*/

#ifndef CDIGGINS_ANY_HPP
#define CDIGGINS_ANY_HPP

//#define ANY_IMPLICIT_CASTING    // added to enable implicit casting

#include <stdexcept>
#include <typeinfo>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <string>

#include "AutoPtr.h"
#include "APITypes.h"
#include "Util/meta_util.h"
//#include "BrowserObjectAPI.h"



#ifdef _WIN32
#pragma warning( disable : 4800 )
#endif

#define BEGIN_CONVERT_MAP(_type_) \
    const std::type_info *type(&get_type()); \
    if (*type == typeid(_type_)) { \
    return cast< _type_ >(); \
    } else

#define END_CONVERT_MAP(_type_) { throw bad_variant_cast(get_type(), typeid(_type_)); }

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

namespace FB
{
    class BrowserObjectAPI;
    struct bad_variant_cast : std::bad_cast {
        bad_variant_cast(const std::type_info& src, const std::type_info& dest)
            : from(src.name()), to(dest.name())
        { }
        virtual const char* what() { 
            return "bad cast"; 
        }
        const char* from;
        const char* to;
    };

    namespace variant_detail
    {
        // function pointer table
        struct fxn_ptr_table {
            const std::type_info& (*get_type)();
            void (*static_delete)(void**);
            void (*clone)(void* const*, void**);
            void (*move)(void* const*,void**);
            bool (*less)(void* const*, void* const*);
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
                static bool lessthan(void* const* left, void* const* right) {
                    T l(*reinterpret_cast<T const*>(left));
                    T r(*reinterpret_cast<T const*>(right));

                    return l < r;
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
                static bool lessthan(void* const* left, void* const* right) {
                    return **reinterpret_cast<T* const*>(left) < **reinterpret_cast<T* const*>(right);
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
                    , fxns<is_small>::template type<T>::lessthan
                };
                return &static_table;
            }
        };

        struct empty {
            bool operator<(const empty& rh) const
            {
                return false;
            }
            template <typename T>
            bool operator<(const T& rh) const
            {
                return true;
            }
        };
    } // namespace variant_detail

    class variant
    {
    public:
        // structors
        template <typename T>
        variant(const T& x) {
            table = variant_detail::get_table<T>::get();
            if (sizeof(T) <= sizeof(void*)) {
                new(&object) T(x);
            }
            else {        
                object = new T(x);
            }
        }

        variant(const char *x) {
            table = variant_detail::get_table<variant_detail::empty>::get();
            object = NULL;
            assign(x);
        }

        variant() {
            table = variant_detail::get_table<variant_detail::empty>::get();
            object = NULL;
        }

        // Utilize the assignment operator
        variant(const variant& x) {
            *this = x;
        }

        ~variant() {
            table->static_delete(&object);
        }

        // assignment
        variant& assign(const variant& x) {
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

        variant& assign(const char *x) {
            return assign(std::string(x));
        }

        template <typename T>
        variant& assign(const T& x)
        {
            // are we copying between the same type?
            variant_detail::fxn_ptr_table* x_table = variant_detail::get_table<T>::get();
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
        variant& operator=(T const& x) {
            return assign(x);
        }

        variant& operator=(variant const& rh) {
            table = variant_detail::get_table<variant_detail::empty>::get();
            return assign(rh);
        }

        // utility functions
        variant& swap(variant& x) {
            std::swap(table, x.table);
            std::swap(object, x.object);
            return *this;
        }

        // comparison function
        bool operator<(const variant& rh) const
        {
            return lessthan(rh);
        }

        bool lessthan(const variant& rh) const {
            if (get_type() == rh.get_type()) {
                return table->less(&object, &rh.object);
            } else {
                std::string left = get_type().name();
                std::string right = rh.get_type().name();
                return left < right;
            }
        }

        const std::type_info& get_type() const {
            return table->get_type();
        }

        template<typename T>
        bool is_of_type() const {
            return (get_type() == typeid(T));
        }

        template<typename T>
        const T& cast() const {
            if (get_type() != typeid(T)) {
                throw bad_variant_cast(get_type(), typeid(T));
            }
            if (sizeof(T) <= sizeof(void*)) {
                return *reinterpret_cast<T const*>(&object);
            }
            else {
                return *reinterpret_cast<T const*>(object);
            }
        }

        template<typename T>        
#ifdef FB_PORTABLE_META
        typename FB::meta::disable_for_containers<T, const T>::type
#else
        const T
#endif
        convert_cast() const
        {
            return convert_cast_impl<T>();
        }

#ifdef FB_PORTABLE_META
        template<class Cont>
        typename FB::meta::enable_for_non_assoc_containers<Cont, const Cont>::type
        convert_cast() const;        
        template<class Dict>
        typename FB::meta::enable_for_pair_assoc_containers<Dict, const Dict>::type
        convert_cast() const;
#endif

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
            return table == variant_detail::get_table<variant_detail::empty>::get();
        }

        void reset()
        {
            if (empty()) return; 
            table->static_delete(&object);
            table = variant_detail::get_table<variant_detail::empty>::get();
            object = NULL;
        }

    private:
        template<typename T>
        const T convert_cast_impl() const {
            return cast<T>();
        }

        // fields
        variant_detail::fxn_ptr_table* table;      
        void* object;
    };

    // boost::any-like casting
    template<typename T>
    inline T* variant_cast(variant* this_) {
        if (this_->get_type() != typeid(T)) {
            throw bad_variant_cast(this_->get_type(), typeid(T));
        }
        if (sizeof(T) <= sizeof(void*)) {
            return reinterpret_cast<T*>(&this_->object);
        }
        else {
            return reinterpret_cast<T*>(this_->object);
        }
    }

    template<typename T>
    inline T const* variant_cast(variant const* this_) {
        return variant_cast<T>(const_cast<variant*>(this_));
    }

    template<typename T>
    inline T const& variant_cast(variant const& this_){
        return *variant_cast<T>(const_cast<variant*>(&this_));
    }
    
    template<> inline const void variant::convert_cast_impl<void>() const {
        return;
    }
    
    template<> inline const variant variant::convert_cast_impl<variant>() const {
        return *this;
    }
    
    template<> inline const int variant::convert_cast_impl<int>() const {
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
    
    template<> inline const double variant::convert_cast_impl<double>() const {
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
    
    template<> inline const float variant::convert_cast_impl<float>() const {
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
    
    template<> inline const long variant::convert_cast_impl<long>() const {
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
    
    template<> inline const short variant::convert_cast_impl<short>() const {
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
    
    template<> inline const std::string variant::convert_cast_impl<std::string>() const {
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
    
    template<> inline const bool variant::convert_cast_impl<bool>() const {
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
}

#endif // CDIGGINS_ANY_HPP
