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

#pragma once
#ifndef CDIGGINS_ANY_HPP
#define CDIGGINS_ANY_HPP

//#define ANY_IMPLICIT_CASTING    // added to enable implicit casting

#include <boost/cstdint.hpp>
#include <stdexcept>
#include <typeinfo>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <string>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>

#include "APITypes.h"
#include "Util/meta_util.h"
#include "utf8_tools.h"
//#include "JSObject.h"



#ifdef _WIN32
#pragma warning( disable : 4800 )
#endif

#define FB_BEGIN_CONVERT_MAP(_type_) \
    const std::type_info *type(&get_type()); \
    if (*type == typeid(_type_)) { \
    return cast< _type_ >(); \
    } else

#define FB_END_CONVERT_MAP(_type_) { throw bad_variant_cast(get_type(), typeid(_type_)); }
#define FB_END_CONVERT_MAP_NO_THROW(_type_) {}

#define FB_CONVERT_ENTRY_SIMPLE(_type_, _srctype_)             \
    if ( *type == typeid( _srctype_ ) ) {              \
    return static_cast< _type_ >( cast< _srctype_ >() );\
    } else

#define FB_CONVERT_ENTRY_COMPLEX_BEGIN(_srctype_, _var_) \
    if (*type == typeid(_srctype_)) { \
    _srctype_ _var_ = cast<_srctype_>();

#define FB_CONVERT_ENTRY_COMPLEX_END() \
    } else

#define FB_CONVERT_ENTRY_NUMERIC(_type_, _srctype_) \
    if (*type == typeid(_srctype_)) { \
        try { \
            return boost::numeric_cast<_type_>(cast<_srctype_>());\
        } catch (const boost::numeric::bad_numeric_cast& ) { \
            throw bad_variant_cast(get_type(), typeid(_type_)); \
        } \
    } else

#define FB_CONVERT_ENTRY_FROM_STRING(_type_, _srctype_) \
    if (*type == typeid(_srctype_)) { \
        typedef _srctype_::value_type char_type; \
        std::basic_istringstream<char_type> iss(cast<_srctype_>()); \
        _type_ to; \
        if (iss >> to) { \
            return to; \
        } else { \
            throw bad_variant_cast(get_type(), typeid(_type_)); \
        } \
    } else

#define FB_CONVERT_ENTRY_FROM_STRING_TYPE(_type_, _srctype_) \
    if (*type == typeid(_srctype_)) { \
        typedef _type_::value_type char_type; \
        std::basic_ostringstream<char_type> oss; \
        if (oss << cast<_srctype_>()) { \
            return oss.str(); \
        } else { \
            throw bad_variant_cast(get_type(), typeid(_type_)); \
        } \
    } else

#define FB_CONVERT_ENTRY_TO_STRING(_srctype_)  FB_CONVERT_ENTRY_FROM_STRING_TYPE(std::string , _srctype_)
#define FB_CONVERT_ENTRY_TO_WSTRING(_srctype_) FB_CONVERT_ENTRY_FROM_STRING_TYPE(std::wstring, _srctype_)

namespace FB
{
    class JSObject;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @exception bad_variant_cast
    ///
    /// @brief  Thrown when variant::cast<type> or variant::convert_cast<type> fails because the
    /// 		type of the value stored in the variant is not compatible with the operation
    ////////////////////////////////////////////////////////////////////////////////////////////////////
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

        struct null {
            bool operator<(const null& rh) const
            {
                return false;
            }
            template <typename T>
            bool operator<(const T& rh) const
            {
                return true;
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  variant
    ///
    /// @brief  Accepts any datatype, used in all interactions with javascript.  Provides tools for
    /// 		getting back out the type you put in or for coercing that type into another type
    /// 		(if possible).
    ///
    /// variant is the most versatile and fundamental class in FireBreath.  Any type can be assigned
    /// to a variant, and you can get that type back out using cast(), like so:
    /// @code
    /// 	 variant a = 5;
    /// 	 int i_a = a.cast<int>();
    /// @endcode
    /// 		
    /// Basic type conversion can be handled using convert_cast(), like so:
    /// @code
    /// 	 variant str = "5";
    /// 	 int i_a = a.convert_cast<int>();
    /// @endcode
    /// 
    /// JSAPIAuto relies heavily on the ability of variant to convert_cast effectively for all type
    /// conversion.  If the type conversion fails, a FB::bad_variant_cast exception will be thrown.
    /// 
    /// @note If you assign a char* to variant it will be automatically converted to a std::string before
    /// 	  the assignment.
    /// @note If you assign a wchar_t* to variant it will be automatically converted to a std::wstring
    /// 	  before the assignment
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class variant
    {
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template <typename T> variant::variant(const T& x)
        ///
        /// @brief  Templated constructor to allow any arbitrary type
        ///
        /// @param  x   The value 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
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

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn variant::variant(const wchar_t *x)
        ///
        /// @brief  Constructor with a wide string. 
        ///
        /// @param  x   The string value
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        variant(const wchar_t *x) {
            table = variant_detail::get_table<variant_detail::empty>::get();
            object = NULL;
            assign(x);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn variant::variant(const char *x)
        ///
        /// @brief  Constructor with a string. 
        ///
        /// @param  x   The string value
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        variant(const char *x) {
            table = variant_detail::get_table<variant_detail::empty>::get();
            object = NULL;
            assign(x);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn variant::variant()
        ///
        /// @brief  Default constructor initializes the varient to an empty value
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        variant() {
            table = variant_detail::get_table<variant_detail::empty>::get();
            object = NULL;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn variant::variant(const variant& x)
        ///
        /// @brief  Copy constructor. 
        ///
        /// @param  x   The variant to copy 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        variant(const variant& x) {
            *this = x;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn variant::~variant()
        ///
        /// @brief  Finaliser. Deletes the value from memory as part of cleanup
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        ~variant() {
            table->static_delete(&object);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn variant& variant::assign(const variant& x)
        ///
        /// @brief  Assigns a new value from another variant
        ///
        /// @param  x   The variant to copy. 
        ///
        /// @return *this
        ////////////////////////////////////////////////////////////////////////////////////////////////////
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

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn variant& variant::assign(const char *x)
        ///
        /// @brief  Assigns a string value as a std::string from a const char*
        ///
        /// @param  x   The string value 
        ///
        /// @return *this
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        variant& assign(const char *x) {
            return assign(std::string(x));
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn variant& variant::assign(const wchar_t *x)
        ///
        /// @brief  Assigns a wide string value as a std::wstring from a const wchar_t*
        ///
        /// @param  x   The string value 
        ///
        /// @return *this
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        variant& assign(const wchar_t *x) {
            return assign(std::wstring(x));
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template <typename T> variant& variant::assign(const T& x)
        ///
        /// @brief  Assigns a value of arbitrary type
        ///
        /// @param  x   The new value 
        ///
        /// @return *this
        ////////////////////////////////////////////////////////////////////////////////////////////////////
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

        // assignment copy operator
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

        // comparison function
        bool lessthan(const variant& rh) const {
            if (get_type() == rh.get_type()) {
                return table->less(&object, &rh.object);
            } else {
                std::string left = get_type().name();
                std::string right = rh.get_type().name();
                return left < right;
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn const std::type_info& variant::get_type() const
        ///
        /// @brief  Gets the type of the value stored in variant
        ///
        /// @return The type that can be compared with typeid()
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        const std::type_info& get_type() const {
            return table->get_type();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template<typename T> bool variant::is_of_type() const
        ///
        /// @brief  Query if this object is of a particular type. 
        /// 		
        /// Example:
        /// @code
        /// 	 if (value.is_of_type<int>())
        /// 	 {
        /// 	    // Do something
        /// 	 }
        /// @endcode
        ///
        /// @return true if of type, false if not. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template<typename T>
        bool is_of_type() const {
            return (get_type() == typeid(T));
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template<typename T> const T& variant::cast() const
        ///
        /// @brief  returns the value cast as the given type; throws bad_variant_type if that type is
        /// 		not the type of the value stored in variant
        ///
        /// @exception  bad_variant_cast    Thrown when bad variant cast. 
        ///
        /// @return value of type T
        ////////////////////////////////////////////////////////////////////////////////////////////////////
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

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template<typename T> typename FB::meta::disable_for_containers_and_numbers<T, const T>::type variant::convert_cast() const
        ///
        /// @brief  Converts the stored value to the requested type *if possible* and returns the resulting
        /// 		value.  If the conversion is not possible, throws bad_variant_cast
        /// 		
        /// Supported destination types include:
        ///   - all numeric types
        ///   - std::string (stored in UTF8 if needed)
        ///   - std::wstring (converted from UTF8 if needed)
        ///   - bool
        ///   - STL container types (from a FB::JSObjectPtr containing an array javascript object)
        ///   - STL dict types (from a FB::JSObjectPtr containing a javascript object)
        ///
        /// @return converted value of the specified type
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template<typename T>        
        typename FB::meta::disable_for_containers_and_numbers<T, const T>::type
        convert_cast() const
        {
            return convert_cast_impl<T>();
        }

        template<class T>
        typename FB::meta::enable_for_numbers<T, T>::type
        convert_cast() const;
        
        template<class Cont>
        typename FB::meta::enable_for_non_assoc_containers<Cont, const Cont>::type
        convert_cast() const;
        
        template<class Dict>
        typename FB::meta::enable_for_pair_assoc_containers<Dict, const Dict>::type
        convert_cast() const;

        // implicit casting is disabled by default for compatibility with boost::any 
#ifdef ANY_IMPLICIT_CASTING
        // automatic casting operator
        template<typename T>
        operator T() const {
            return convert_cast<T>();
        }
#endif // implicit casting

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn bool variant::empty() const
        ///
        /// @brief  Returns true if the variant is empty (has not been assigned a value or has been reset)
        ///
        /// @return true if empty, false if not 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        bool empty() const {
            //return object == NULL;
            return table == variant_detail::get_table<variant_detail::empty>::get();
        }

        bool is_null() const {
            return table == variant_detail::get_table<variant_detail::null>::get();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void variant::reset()
        ///
        /// @brief  Frees any value assigned and resets the variant to empty state
        ///
        /// @author Richard Bateman
        /// @date   10/15/2010
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void reset()
        {
            if (empty()) return; 
            table->static_delete(&object);
            table = variant_detail::get_table<variant_detail::empty>::get();
            object = NULL;
        }
        
        // boost::any-like casting
        template<typename T>
        inline T* variant_cast() 
        {
            if (get_type() != typeid(T)) {
                throw bad_variant_cast(get_type(), typeid(T));
            }
            if (sizeof(T) <= sizeof(void*)) {
                return reinterpret_cast<T*>(&object);
            }
            else {
                return reinterpret_cast<T*>(object);
            }
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
    inline T* variant_cast(variant* v) {
        return v->variant_cast<T>();
    }

    template<typename T>
    inline T const* variant_cast(variant const* v) {
        return variant_cast<T>(const_cast<variant*>(v));
    }

    template<typename T>
    inline T const& variant_cast(variant const& v){
        return *variant_cast<T>(const_cast<variant*>(&v));
    }
    
    template<> inline const void variant::convert_cast_impl<void>() const {
        return;
    }
    
    template<> inline const variant variant::convert_cast_impl<variant>() const {
        return *this;
    }
    
    template<class T>
    inline typename FB::meta::enable_for_numbers<T, T>::type
    variant::convert_cast() const {
        FB_BEGIN_CONVERT_MAP(T)
        FB_CONVERT_ENTRY_NUMERIC(T, char)
        FB_CONVERT_ENTRY_NUMERIC(T, unsigned char)
        FB_CONVERT_ENTRY_NUMERIC(T, short)
        FB_CONVERT_ENTRY_NUMERIC(T, unsigned short)
        FB_CONVERT_ENTRY_NUMERIC(T, int)
        FB_CONVERT_ENTRY_NUMERIC(T, unsigned int)
        FB_CONVERT_ENTRY_NUMERIC(T, long)
        FB_CONVERT_ENTRY_NUMERIC(T, unsigned long)
        FB_CONVERT_ENTRY_NUMERIC(T, float)
        FB_CONVERT_ENTRY_NUMERIC(T, double)
        FB_CONVERT_ENTRY_COMPLEX_BEGIN(bool, bval);
            // we handle bool here specifically because the numeric_cast produces warnings
            return static_cast<T>(bval ? 1 : 0);
        FB_CONVERT_ENTRY_COMPLEX_END();
        FB_CONVERT_ENTRY_FROM_STRING(T, std::string)
        FB_CONVERT_ENTRY_FROM_STRING(T, std::wstring)
        FB_END_CONVERT_MAP(T)
    }
    
    template<> inline const std::string variant::convert_cast_impl<std::string>() const {
        FB_BEGIN_CONVERT_MAP(std::string);
        FB_CONVERT_ENTRY_TO_STRING(double);
        FB_CONVERT_ENTRY_TO_STRING(float);
        FB_CONVERT_ENTRY_TO_STRING(int);
        FB_CONVERT_ENTRY_TO_STRING(unsigned int);
        FB_CONVERT_ENTRY_COMPLEX_BEGIN(bool, bval);
        return bval ? "true" : "false";
        FB_CONVERT_ENTRY_COMPLEX_END();
        FB_CONVERT_ENTRY_COMPLEX_BEGIN(std::wstring, str);
        return wstring_to_utf8(str);
        FB_CONVERT_ENTRY_COMPLEX_END();
        FB_CONVERT_ENTRY_TO_STRING(long);
        FB_CONVERT_ENTRY_TO_STRING(unsigned long);
        FB_CONVERT_ENTRY_TO_STRING(short);
        FB_CONVERT_ENTRY_TO_STRING(unsigned short);
        FB_CONVERT_ENTRY_TO_STRING(char);
        FB_CONVERT_ENTRY_TO_STRING(unsigned char);
        FB_CONVERT_ENTRY_TO_STRING(boost::int64_t);
        FB_CONVERT_ENTRY_TO_STRING(boost::uint64_t);
        FB_END_CONVERT_MAP(std::string);
    }

    template<> inline const std::wstring variant::convert_cast_impl<std::wstring>() const {
        FB_BEGIN_CONVERT_MAP(std::wstring);
        FB_CONVERT_ENTRY_TO_WSTRING(double);
        FB_CONVERT_ENTRY_TO_WSTRING(float);
        FB_CONVERT_ENTRY_TO_WSTRING(int);
        FB_CONVERT_ENTRY_TO_WSTRING(unsigned int);
        FB_CONVERT_ENTRY_COMPLEX_BEGIN(bool, bval);
        return bval ? L"true" : L"false";
        FB_CONVERT_ENTRY_COMPLEX_END();
        FB_CONVERT_ENTRY_COMPLEX_BEGIN(std::string, str);
        return utf8_to_wstring(str);
        FB_CONVERT_ENTRY_COMPLEX_END();
        FB_CONVERT_ENTRY_TO_WSTRING(long);
        FB_CONVERT_ENTRY_TO_WSTRING(unsigned long);
        FB_CONVERT_ENTRY_TO_WSTRING(short);
        FB_CONVERT_ENTRY_TO_WSTRING(unsigned short);
        FB_CONVERT_ENTRY_TO_WSTRING(char);
        FB_CONVERT_ENTRY_TO_WSTRING(unsigned char);
        FB_END_CONVERT_MAP(std::wstring);
    }
    
    template<> inline const bool variant::convert_cast_impl<bool>() const {
        FB_BEGIN_CONVERT_MAP(bool);
        FB_CONVERT_ENTRY_COMPLEX_BEGIN(std::string, str);
        std::transform(str.begin(), str.end(), str.begin(), ::tolower); 
        return (str == "y" || str == "1" || str == "yes" || str == "true" || str == "t");
        FB_CONVERT_ENTRY_COMPLEX_END();
        FB_CONVERT_ENTRY_COMPLEX_BEGIN(std::wstring, str);
        std::transform(str.begin(), str.end(), str.begin(), ::tolower); 
        return (str == L"y" || str == L"1" || str == L"yes" || str == L"true" || str == L"t");
        FB_CONVERT_ENTRY_COMPLEX_END();
        FB_END_CONVERT_MAP_NO_THROW(short);
        
        return convert_cast_impl<long>();
    }
}

#undef FB_BEGIN_CONVERT_MAP
#undef FB_END_CONVERT_MAP
#undef FB_END_CONVERT_MAP_NO_THROW
#undef FB_CONVERT_ENTRY_SIMPLE
#undef FB_CONVERT_ENTRY_NUMERIC
#undef FB_CONVERT_ENTRY_TO_STRING
#undef FB_CONVERT_ENTRY_TO_WSTRING
#undef FB_CONVERT_ENTRY_FROM_STRING
#undef FB_CONVERT_ENTRY_FROM_STRING_TYPE
#undef FB_CONVERT_ENTRY_COMPLEX_BEGIN
#undef FB_CONVERT_ENTRY_COMPLEX_END

#endif // CDIGGINS_ANY_HPP
