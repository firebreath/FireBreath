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

#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/variant/variant_fwd.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/logic/tribool.hpp>

#include "APITypes.h"
#include "Util/meta_util.h"
#include "utf8_tools.h"
#include "variant_conversions.h"
//#include "JSObject.h"



#ifdef _WIN32
#pragma warning(push)
#pragma warning( disable : 4800 )
#endif

#define FB_BEGIN_CONVERT_MAP(_type_) \
    const std::type_info *type(&var.get_type()); \
    if (*type == typeid(_type_)) { \
    return var.cast< _type_ >(); \
    } else

#define FB_END_CONVERT_MAP(_type_) { throw bad_variant_cast(var.get_type(), typeid(_type_)); }
#define FB_END_CONVERT_MAP_NO_THROW(_type_) {}

#define FB_CONVERT_ENTRY_SIMPLE(_type_, _srctype_)             \
    if ( *type == typeid( _srctype_ ) ) {              \
    return static_cast< _type_ >( var.cast< _srctype_ >() );\
    } else

#define FB_CONVERT_ENTRY_COMPLEX_BEGIN(_srctype_, _var_) \
    if (*type == typeid(_srctype_)) { \
    _srctype_ _var_ = var.cast<_srctype_>();

#define FB_CONVERT_ENTRY_COMPLEX_END() \
    } else

#define FB_CONVERT_ENTRY_NUMERIC(_type_, _srctype_) \
    if (*type == typeid(_srctype_)) { \
        try { \
            return boost::numeric_cast<_type_>(var.cast<_srctype_>());\
        } catch (const boost::numeric::bad_numeric_cast& ) { \
            throw bad_variant_cast(var.get_type(), typeid(_type_)); \
        } \
    } else

#define FB_CONVERT_ENTRY_FROM_STRING(_type_, _srctype_) \
    if (*type == typeid(_srctype_)) { \
        typedef _srctype_::value_type char_type; \
        std::basic_istringstream<char_type> iss(var.cast<_srctype_>()); \
        _type_ to; \
        if (iss >> to) { \
            return to; \
        } else { \
            throw bad_variant_cast(var.get_type(), typeid(_type_)); \
        } \
    } else

#define FB_CONVERT_ENTRY_FROM_STRING_TYPE(_type_, _srctype_) \
    if (*type == typeid(_srctype_)) { \
        typedef _type_::value_type char_type; \
        std::basic_ostringstream<char_type> oss; \
        if (oss << var.cast<_srctype_>()) { \
            return oss.str(); \
        } else { \
            throw bad_variant_cast(var.get_type(), typeid(_type_)); \
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
    ///         type of the value stored in the variant is not compatible with the operation
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

    class variant;

    template <class T>
    variant make_variant(T t);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  variant
    ///
    /// @brief  Accepts any datatype, used in all interactions with javascript.  Provides tools for
    ///         getting back out the type you put in or for coercing that type into another type
    ///         (if possible).
    ///
    /// variant is the most versatile and fundamental class in FireBreath.  Any type can be assigned
    /// to a variant, and you can get that type back out using cast(), like so:
    /// @code
    ///      variant a = 5;
    ///      int i_a = a.cast<int>();
    /// @endcode
    ///         
    /// Basic type conversion can be handled using convert_cast(), like so:
    /// @code
    ///      variant str = "5";
    ///      int i_a = a.convert_cast<int>();
    /// @endcode
    /// 
    /// JSAPIAuto relies heavily on the ability of variant to convert_cast effectively for all type
    /// conversion.  If the type conversion fails, a FB::bad_variant_cast exception will be thrown.
    /// 
    /// @note If you assign a char* to variant it will be automatically converted to a std::string before
    ///       the assignment.
    /// @note If you assign a wchar_t* to variant it will be automatically converted to a std::wstring
    ///       before the assignment
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
        variant(const T& x, bool) {
            table = variant_detail::get_table<variant_detail::empty>::get();
            object = NULL;
            assign(x, true);
        }

        template <typename T>
        variant(const T& x) {
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

        template<class T>
        variant& assign(const T x) {
            *this = make_variant(x);
            // If you get an error that there are no overloads that could convert all the argument
            // types for this line, you are trying to use a type that is not known to FireBreath.
            // First, make sure you really want to use this type! If you aren't doing this on
            // purpose, double check your type, because it's wrong!
            //
            // Alternately, if you mean to do this there are two options:
            // 1. You can create a variant or call .assign on a variant with a second bool
            //    parameter, like so:
            //        FB::variant tmp((void*)0x12, true); // Forces creation with unknown type
            //        tmp.assign((int*)0x32, true); // Forces assignment of unknown type
            //
            // 2. You could either create your own assignment function by either overriding
            //    FB::make_variant or FB::variant_detail::conversion::make_variant.
            //
            return *this;
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
        variant& assign(const T& x, bool)
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
        ///      if (value.is_of_type<int>())
        ///      {
        ///         // Do something
        ///      }
        /// @endcode
        ///
        /// @return true if of type, false if not. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template<typename T>
        bool can_be_type() const {
            if (get_type() == typeid(T))
                return true;
            try {
                // See if it can be that type by converting
                convert_cast<T>();
                return true;
            } catch (...) {
                return false;
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template<typename T> bool variant::is_of_type() const
        ///
        /// @brief  Query if this object is of a particular type. 
        ///         
        /// Example:
        /// @code
        ///      if (value.is_of_type<int>())
        ///      {
        ///         // Do something
        ///      }
        /// @endcode
        ///
        /// @return true if of type, false if not. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template<typename T>
        bool is_of_type() const {
            return (get_type() == typeid(T));
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template<typename T> T& variant::cast() const
        ///
        /// @brief  returns the value cast as the given type; throws bad_variant_type if that type is
        ///         not the type of the value stored in variant
        ///
        /// @exception  bad_variant_cast    Thrown when bad variant cast. 
        ///
        /// @return value of type T
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template<typename T>
        T& cast() {
            if (get_type() != typeid(T)) {
                throw bad_variant_cast(get_type(), typeid(T));
            }
            if (sizeof(T) <= sizeof(void*)) {
                return *reinterpret_cast<T*>(&object);
            }
            else {
                return *reinterpret_cast<T*>(object);
            }
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

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template<typename T> typename FB::meta::disable_for_containers_and_numbers<T, const T>::type variant::convert_cast() const
        ///
        /// @brief  Converts the stored value to the requested type *if possible* and returns the resulting
        ///         value.  If the conversion is not possible, throws bad_variant_cast
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
        const T convert_cast() const;

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
        
    private:
        template<typename T>
        const T convert_cast_impl() const {
            return cast<T>();
        }

        // fields
        variant_detail::fxn_ptr_table* table;      
        void* object;
    };

    template <>
    inline const std::string variant::convert_cast<std::string>() const {
        variant var = *this;
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

    template<>
    inline const std::wstring variant::convert_cast<std::wstring>() const {
        variant var = *this;
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
    
    template<>
    inline const bool variant::convert_cast<bool>() const {
        variant var = *this;
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
        
        return convert_cast<long>();
    }

    namespace variant_detail {
        namespace conversion {
            ///////////////////////////////////////////////////
            // variant assign conversion functions
            //
            // These functions are called to process any
            // values assigned to the variant.  For example,
            // all const char* parameters are converted to
            // std::string
            ///////////////////////////////////////////////////
            template <class T>
            typename boost::enable_if<
                boost::mpl::or_<
                    boost::mpl::or_<
                        boost::mpl::or_<
                            boost::is_same<std::vector<variant>, T>,
                            boost::is_same<std::map<std::string, variant>, T>
                        >,
                        boost::mpl::or_<
                            boost::is_same<std::wstring, T>,
                            boost::is_same<std::string, T>
                        >
                    >,
                    boost::mpl::or_<
                        boost::is_same<variant_detail::empty, T>,
                        boost::is_same<variant_detail::null, T>
                    >
                >, variant>::type
            make_variant(const T& t) {
                return variant(t, true);
            }

            template <class T>
            variant make_variant(const boost::optional<T>& val) {
                if (val)
                    return variant(*val);
                else
                    return variant();
            }

            inline variant make_variant(const FB::JSAPIWeakPtr& ptr) {
                return variant(ptr, true);
            }
            
            variant make_variant(const boost::tribool& val);
            boost::tribool convert_variant( const FB::variant& var, const type_spec<boost::tribool>& );

            template <class T>
            typename boost::enable_if<
                boost::mpl::and_<
                    boost::is_convertible<T, int>,
                    boost::mpl::not_<boost::is_arithmetic<T> >
                >, variant>::type
            make_variant(const T t) {
                return variant(static_cast<int>(t), true);
            }

            template <class T>
            typename boost::enable_if<boost::is_arithmetic<T>, variant>::type
            make_variant(const T t) {
                return variant(t, true);
            }

            variant make_variant(const char* x);
            variant make_variant(const wchar_t* x);
            ///////////////////////////////////////////////////
            // variant convert_cast helpers
            //
            // These functions are called to process any
            // values assigned to the variant.  For example,
            // all const char* parameters are converted to
            // std::string
            ///////////////////////////////////////////////////
            struct boost_variant_to_FBVariant
                : public boost::static_visitor<FB::variant> {
                template <typename T>
                FB::variant operator()(T inVal) const {
                    return FB::variant(inVal);
                }
            };

            template <class T>
            typename boost::enable_if<
                FB::meta::is_boost_variant<T>, variant
            >::type
            make_variant(T& inVal) {
                return boost::apply_visitor(boost_variant_to_FBVariant(), inVal);
            }

            template <typename V>
            struct FBVariant_to_boost_variant {
                V* res;
                bool* found;
                const variant& val;
                FBVariant_to_boost_variant(const variant& val, bool* b, V* v) : res(v), found(b), val(val) {}

                template <typename T>
                void operator()(type_spec<T>&) {
                    if (typeid(T) == val.get_type()) {
                        // If we find an exact type match, use it
                        *res = val.cast<T>();
                        *found = true;
                        return;
                    }
                    else if (*found) return; // When we find the correct cast we can be done
                    try {
                        // If we haven't found an exact match, go for the next best match
                        *res = val.convert_cast<T>();
                        *found = true;
                    } catch (const FB::bad_variant_cast&) {
                    }
                }
            };
            
            template <class T>
            T convert_variant(const variant& var, const type_spec<T>,
                typename boost::enable_if<FB::meta::is_boost_variant<T> >::type*p = 0) {
                bool found(false);
                T res;
                FBVariant_to_boost_variant<T> converter(var, &found, &res);
                boost::mpl::for_each<typename T::types, type_spec<boost::mpl::_1> >(converter);
                if (found)
                    return res;
                else
                    throw FB::bad_variant_cast(var.get_type(), typeid(T));
            }
            
            const void convert_variant(const variant&, const type_spec<void>);
            const FB::FBNull convert_variant(const variant&, const type_spec<FBNull>);
            const FB::FBVoid convert_variant(const variant&, const type_spec<FBVoid>);
            const variant& convert_variant(const variant& var, const type_spec<variant>);
            
            template<typename T>
            boost::optional<T> convert_variant(const variant& var, const type_spec<boost::optional<T> >) {
                if (var.is_null() || var.empty())
                    return boost::optional<T>();

                return var.convert_cast<T>();
            }

            template<typename T>
            typename FB::meta::enable_for_numbers<T, T>::type
            convert_variant(const variant& var, const type_spec<T>&) {
                FB_BEGIN_CONVERT_MAP(T)
                FB_CONVERT_ENTRY_NUMERIC(T, char)
                FB_CONVERT_ENTRY_NUMERIC(T, unsigned char)
                FB_CONVERT_ENTRY_NUMERIC(T, short)
                FB_CONVERT_ENTRY_NUMERIC(T, unsigned short)
                FB_CONVERT_ENTRY_NUMERIC(T, int)
                FB_CONVERT_ENTRY_NUMERIC(T, unsigned int)
                FB_CONVERT_ENTRY_NUMERIC(T, long)
                FB_CONVERT_ENTRY_NUMERIC(T, unsigned long)
                FB_CONVERT_ENTRY_NUMERIC(T, long long)
                FB_CONVERT_ENTRY_NUMERIC(T, unsigned long long)
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
        }
    }
    template<typename T>
    const T variant::convert_cast() const
    {
        return variant_detail::conversion::convert_variant(*this, variant_detail::conversion::type_spec<T>());
    }
    template <class T>
    variant make_variant(T t)
    {
        // If you got an error on this line, you are trying to assign an unsupported type to 
        // FB::variant! If you're certain you want to do this then you should use the constructor
        // or assign method that takes a bool.  e.g.: variant tmp(myWeirdType, true);
        return variant_detail::conversion::make_variant(t);
    }
}

#ifdef _WIN32
#pragma warning(pop)
#endif

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

