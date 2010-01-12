/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    December 22, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#ifndef H_META_UTIL_IMPL_22122009
#define H_META_UTIL_IMPL_22122009

#include <boost/utility/enable_if.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/type_traits.hpp>

namespace FB { namespace meta { namespace detail
{
#ifdef _WINDOWS
    template<typename C>
    struct is_container_impl
    {
        typedef char yes;
        typedef char (&no)[2];

        template<typename U, size_t (U::*)(const int&) const> struct sfinae {};
        template<typename U> static yes test(typename U::const_iterator = U().begin());
        template<typename U> static no  test(...);

        enum { value = (sizeof(test<C>()) == sizeof(yes)) };
        typedef boost::mpl::bool_<value> type;
    };

    template<typename C>
    struct is_assoc_impl
    {
        typedef char yes;
        typedef char (&no)[2];

        template<typename U, size_t (U::*)(const int&) const> struct sfinae {};
        template<typename U> static yes test(typename U::key_type = U::key_type());
        template<typename U> static no  test(...);

        enum { value = (sizeof(test<C>()) == sizeof(yes)) };
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

    template<class T>
    struct is_container
        : boost::mpl::and_< 
            boost::mpl::not_< is_pseudo_container<T> >,
            typename is_container_impl<T>::type >::type
    {};

    template<typename C>
    struct is_assoc_container
        : boost::mpl::and_<
            typename is_assoc_impl<C>::type, 
            is_container<C> >::type
    {};

    template<typename C>
    struct is_non_assoc_container
        : boost::mpl::and_< 
            boost::mpl::not_<is_assoc_container<C> >, 
            is_container<C> >::type
    {};

    // enable_if helpers:
    //   T - the type to compare
    //   R - the return type

    template<class T, typename R>
    struct enable_for_containers_impl
        : boost::enable_if<is_container<T>, R>
    {};

    template<class T, typename R>
    struct disable_for_containers_impl 
        : boost::disable_if<is_container<T>, R>
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
    struct enable_for_non_assoc_containers_impl 
        : boost::enable_if<is_non_assoc_container<T>, R>
    {};

    template<class T, typename R>
    struct disable_for_non_assoc_containers_impl 
        : boost::disable_if<is_non_assoc_container<T>, R>
    {};
#endif
}; }; };

#endif