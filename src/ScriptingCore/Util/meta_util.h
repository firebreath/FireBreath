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
#ifndef H_META_UTIL_22122009
#define H_META_UTIL_22122009

#include <boost/optional.hpp>
#include "meta_util_impl.h"

namespace FB { namespace meta 
{
    // get the plain type of T (without pointers, references, ...)
    template<typename T>
    struct plain_type 
        : FB::meta::detail::plain_type<T> {};

    ////////////////////////////////////////////////
    // is optional - a boost::optional<T> type

    template <typename T>
    struct is_optional
        : boost::mpl::false_ {};

    template <typename T>
    struct is_optional< boost::optional<T> >
        : boost::mpl::true_ {};

    ////////////////////////////////////////////////
    // is_boost_variant - is a boost::variant type

    template <typename T>
    struct is_boost_variant
        : boost::mpl::false_ {};

    template <BOOST_VARIANT_ENUM_PARAMS(typename T)>
    struct is_boost_variant< boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> >
        : boost::mpl::true_ {};

    ///////////////////////////////////////////////////////
    // STL style container classification
    //   T - the type to compare

    template<class T>
    struct is_container 
        : FB::meta::detail::is_container<T> {};

    template<class T>
    struct is_assoc_container 
        : FB::meta::detail::is_assoc_container<T> {};

    template<class T>
    struct is_pair_assoc_container
        : FB::meta::detail::is_pair_assoc_container<T> {};

    template<class T>
    struct is_non_assoc_container 
        : FB::meta::detail::is_non_assoc_container<T> {};

    template<class T>
    struct is_pseudo_container 
        : FB::meta::detail::is_pseudo_container<T> {};

    ///////////////////////////////////////////////////////
    // enable_if helpers:
    //   T - the type to compare
    //   R - the return type
    
    template<class T, typename R=void>
    struct enable_for_containers_and_numbers
        : FB::meta::detail::enable_for_containers_and_numbers_impl<T,R> {};
    
    template<class T, typename R=void>
    struct disable_for_containers_and_numbers
        : FB::meta::detail::disable_for_containers_and_numbers_impl<T,R> {};
    
    template<class T, typename R=void>
    struct enable_for_numbers
        : FB::meta::detail::enable_for_numbers_impl<T,R> {};
    
    template<class T, typename R=void>
    struct disable_for_numbers
        : FB::meta::detail::disable_for_numbers_impl<T,R> {};

    template<class T, typename R=void>
    struct enable_for_containers 
        : FB::meta::detail::enable_for_containers_impl<T,R> {};

    template<class T, typename R=void>
    struct disable_for_containers 
        : FB::meta::detail::disable_for_containers_impl<T,R> {};

    template<class T, typename R=void>
    struct enable_for_assoc_containers 
        : FB::meta::detail::enable_for_assoc_containers_impl<T,R> {};

    template<class T, typename R=void>
    struct disable_for_assoc_containers 
        : FB::meta::detail::disable_for_assoc_containers_impl<T,R> {};

    template<class T, typename R=void>
    struct enable_for_pair_assoc_containers 
        : FB::meta::detail::enable_for_pair_assoc_containers_impl<T,R> {};

    template<class T, typename R=void>
    struct disable_for_pair_assoc_containers 
        : FB::meta::detail::disable_for_pair_assoc_containers_impl<T,R> {};

    template<class T, typename R=void>
    struct enable_for_non_assoc_containers 
        : FB::meta::detail::enable_for_non_assoc_containers_impl<T,R> {};

    template<class T, typename R=void>
    struct disable_for_non_assoc_containers 
        : FB::meta::detail::disable_for_non_assoc_containers_impl<T,R> {};
}; };


#endif

