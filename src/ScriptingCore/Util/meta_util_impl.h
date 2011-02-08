/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    December 22, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_META_UTIL_IMPL_22122009
#define H_META_UTIL_IMPL_22122009

#include <utility>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/and.hpp>
#include <boost/type_traits.hpp>
#include <boost/variant/variant_fwd.hpp>

namespace FB { namespace meta { namespace detail
{
    class yes { char c;   };
    class no  { yes y[2]; };

#define FB_HAS_TYPE(Type_, Name_) \
    template<class T> \
    struct Name_ { \
        template<class U> static yes test(typename U::Type_ const*); \
        template<class U> static no  test(...); \
        static const bool value = (sizeof(test<T>(0)) == sizeof(yes)); \
        typedef boost::mpl::bool_<value> type; \
    }

    FB_HAS_TYPE(iterator,        has_type_iterator);
    FB_HAS_TYPE(const_iterator,  has_type_const_iterator);
    FB_HAS_TYPE(key_type,        has_type_key_type);
    FB_HAS_TYPE(value_type,      has_type_value_type);
    FB_HAS_TYPE(mapped_type,     has_type_mapped_type);
//    FB_HAS_TYPE(reference,       has_type_reference);
//    FB_HAS_TYPE(const_reference, has_type_const_reference);
    FB_HAS_TYPE(pointer,         has_type_pointer);
    FB_HAS_TYPE(difference_type, has_type_difference_type);
    FB_HAS_TYPE(size_type,       has_type_size_type);

#undef FB_HAS_TYPE

    template<class C>
    class is_container_impl
    {
    private:
        struct mixin 
        {
            void begin() const;
            void end() const;
            void size() const;
            void max_size() const;
            void empty() const;
            void swap(char);
        };

        struct derivate : public C, public mixin {};
        template<class U, U t> struct helper {};

        template<class U> static no  test_begin(helper<void (mixin::*)() const, &U::begin>*);
        template<class U> static yes test_begin(...);
        template<class U> static no  test_end(helper<void (mixin::*)() const, &U::end>*);
        template<class U> static yes test_end(...);
        template<class U> static no  test_size(helper<void (mixin::*)() const, &U::size>*);
        template<class U> static yes test_size(...);
        template<class U> static no  test_max_size(helper<void (mixin::*)() const, &U::max_size>*);
        template<class U> static yes test_max_size(...);
        template<class U> static no  test_empty(helper<void (mixin::*)() const, &U::empty>*);
        template<class U> static yes test_empty(...);
        template<class U> static no  test_swap(helper<void (mixin::*)(char), &U::swap>*);
        template<class U> static yes test_swap(...);

    public:
        static const bool has_memfun_begin    = (sizeof(yes) == sizeof(test_begin   <derivate>(0)));
        static const bool has_memfun_end      = (sizeof(yes) == sizeof(test_end     <derivate>(0)));
        static const bool has_memfun_size     = (sizeof(yes) == sizeof(test_size    <derivate>(0)));
        static const bool has_memfun_max_size = (sizeof(yes) == sizeof(test_max_size<derivate>(0)));
        static const bool has_memfun_empty    = (sizeof(yes) == sizeof(test_empty   <derivate>(0)));
        static const bool has_memfun_swap     = (sizeof(yes) == sizeof(test_swap    <derivate>(0)));

        static const bool value = 
               has_type_iterator<C>::value 
            && has_type_const_iterator<C>::value
            && has_type_value_type<C>::value
//              && has_type_reference<C>::value
//              && has_type_const_reference<C>::value
            && has_type_pointer<C>::value
            && has_type_difference_type<C>::value
            && has_type_size_type<C>::value
            && has_memfun_begin
            && has_memfun_size
            && has_memfun_max_size
            && has_memfun_empty
            && has_memfun_swap;
        typedef boost::mpl::bool_<value> type;
    };

    template<class C>
    class is_assoc_impl
    {
        struct mixin
        {
            void erase(char);
            void erase(char, char);
            void clear();
            void find(char) const;
            void count(char) const;
            void equal_range(char) const;
        };

        struct derivate : public C, public mixin {};
        template<class U, U t> struct helper {};

        template<class U> static no  test_erase_1(helper<void (mixin::*)(char), &U::erase>*);
        template<class U> static yes test_erase_1(...);
        template<class U> static no  test_erase_2(helper<void (mixin::*)(char, char), &U::erase>*);
        template<class U> static yes test_erase_2(...);
        template<class U> static no  test_clear(helper<void (mixin::*)(), &U::clear>*);
        template<class U> static yes test_clear(...);
        template<class U> static no  test_find(helper<void (mixin::*)(char) const, &U::find>*);
        template<class U> static yes test_find(...);
        template<class U> static no  test_count(helper<void (mixin::*)(char) const, &U::count>*);
        template<class U> static yes test_count(...);
        template<class U> static no  test_equal_range(helper<void (mixin::*)(char) const, &U::equal_range>*);
        template<class U> static yes test_equal_range(...);

    public:
        static const bool has_memfun_erase_1     = (sizeof(yes) == sizeof(test_erase_1    <derivate>(0)));
        static const bool has_memfun_erase_2     = (sizeof(yes) == sizeof(test_erase_2    <derivate>(0)));
        static const bool has_memfun_clear       = (sizeof(yes) == sizeof(test_clear      <derivate>(0)));
        static const bool has_memfun_find        = (sizeof(yes) == sizeof(test_find       <derivate>(0)));
        static const bool has_memfun_count       = (sizeof(yes) == sizeof(test_count      <derivate>(0)));
        static const bool has_memfun_equal_range = (sizeof(yes) == sizeof(test_equal_range<derivate>(0)));

        static const bool value = 
               has_type_key_type<C>::value
            && has_memfun_erase_1
            && has_memfun_erase_2
            && has_memfun_clear
            && has_memfun_find
            && has_memfun_count
            && has_memfun_equal_range;
        typedef boost::mpl::bool_<value> type;
    };

    template<bool has_mapped_type, class T>
    struct check_pair_assoc_value_type
    {
        typedef typename T::key_type Key;
        typedef typename T::mapped_type Mapped;
        typedef typename T::value_type Value;
        typedef std::pair<const Key, Mapped> Pair;

        static const bool value = boost::is_same<Value, Pair>::value;
    };

    template<class T>
    struct check_pair_assoc_value_type<false, T>
    {
        static const bool value = false;
    };

    template<class T>
    class is_pair_assoc_impl
    {
        
    public:
        static const bool has_mapped_type = has_type_mapped_type<T>::value;
        static const bool value_type_is_pair = 
            check_pair_assoc_value_type<has_mapped_type, T>::value;

        static const bool value = value_type_is_pair;
        typedef boost::mpl::bool_<value> type;
    };

    typedef boost::mpl::vector
    <
        std::string, 
        std::wstring
    > pseudo_container_types;

    template<typename T>
    struct plain_type {
        typedef typename boost::remove_const<typename boost::remove_reference<T>::type>::type type;
    };

    template<class T>
    struct is_pseudo_container 
      : boost::mpl::contains<pseudo_container_types, typename plain_type<T>::type>::type
    {};

    ////////////////
    // is_container

    template<bool isClass, class T>
    struct is_container_helper
      : boost::mpl::and_< 
            boost::mpl::not_< is_pseudo_container<T> >,
            typename is_container_impl<T>::type >::type
    {};

    template<class T>
    struct is_container_helper<false, T> 
      : boost::mpl::false_
    {};

    template<class T>
    struct is_container
      : is_container_helper<boost::is_class<T>::value, T>
    {};

    //////////////////////
    // is_assoc_container

    template<bool isClass, class T>
    struct is_assoc_container_helper
      : boost::mpl::and_<
          typename is_assoc_impl<T>::type, 
          is_container<T> >::type
    {};

    template<class T>
    struct is_assoc_container_helper<false, T>
      : boost::mpl::false_
    {};

    template<class T>
    struct is_assoc_container
      : is_assoc_container_helper<boost::is_class<T>::value, T>
    {};

    ///////////////////////////
    // is_pair_assoc_container

    template<bool isClass, class T>
    struct is_pair_assoc_container_helper
      : boost::mpl::and_<
          typename is_pair_assoc_impl<T>::type, 
          is_assoc_container<T> >::type
    {};

    template<class T>
    struct is_pair_assoc_container_helper<false, T>
      : boost::mpl::false_
    {};

    template<class T>
    struct is_pair_assoc_container
      : is_pair_assoc_container_helper<boost::is_class<T>::value, T>
    {};

    //////////////////////////
    // is_non_assoc_container

    template<bool isClass, class T>
    struct is_non_assoc_container_helper
      : boost::mpl::and_< 
          boost::mpl::not_<is_assoc_container<T> >, 
          is_container<T> >::type
    {};

    template<class T>
    struct is_non_assoc_container_helper<false, T>
      : boost::mpl::false_
    {};

    template<class T>
    struct is_non_assoc_container
      : is_non_assoc_container_helper<boost::is_class<T>::value, T>
    {};
    
    ////////////////////////////////////////////////
    // is number - we consider bool as a non-number
    
    template<class T>
    struct is_number
      : boost::mpl::and_<
          boost::is_arithmetic<T>,
          boost::mpl::not_<
            boost::is_same<T, bool> 
          >
        >
    {};

    ///////////////////////////////////////////////////////////////////////////
    // enable_if helpers:
    //   T - the type to compare
    //   R - the return type

    template<class T, typename R>
    struct enable_for_numbers_impl
        : boost::enable_if<is_number<T>, R>
    {};
    
    template<class T, typename R>
    struct disable_for_numbers_impl 
        : boost::disable_if<is_number<T>, R>
    {};
    
    template<class T, typename R>
    struct enable_for_containers_impl
        : boost::enable_if<is_container<T>, R>
    {};

    template<class T, typename R>
    struct disable_for_containers_impl 
        : boost::disable_if<is_container<T>, R>
    {};
    
    template<class T, typename R>
    struct enable_for_containers_and_numbers_impl 
        : boost::enable_if<
            boost::mpl::or_<
              is_container<T>,
              is_number<T> 
            >,
            R
          >
    {};
    
    template<class T, typename R>
    struct disable_for_containers_and_numbers_impl 
        : boost::disable_if<
            boost::mpl::or_<
              is_container<T>,
              is_number<T> 
            >,
            R
          >
    {};

    template<class T, typename R>
    struct enable_for_assoc_containers_impl
        : boost::enable_if<is_assoc_container<T>, R>
    {};

    template<class T, typename R>
    struct disable_for_assoc_containers_impl
        : boost::disable_if<is_assoc_container<T>, R>
    {};

    template<class T, typename R>
    struct enable_for_pair_assoc_containers_impl
        : boost::enable_if<is_pair_assoc_container<T>, R>
    {};

    template<class T, typename R>
    struct disable_for_pair_assoc_containers_impl
        : boost::disable_if<is_pair_assoc_container<T>, R>
    {};

    template<class T, typename R>
    struct enable_for_non_assoc_containers_impl 
        : boost::enable_if<is_non_assoc_container<T>, R>
    {};

    template<class T, typename R>
    struct disable_for_non_assoc_containers_impl 
        : boost::disable_if<is_non_assoc_container<T>, R>
    {};

}; }; };

#endif

