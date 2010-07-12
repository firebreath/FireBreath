/**********************************************************\
Original Author: Georg Fritzsche
Created: 2009-11-07
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/


#ifndef PROPERTY_CONVERTER_H
#define PROPERTY_CONVERTER_H

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/function_types/is_member_function_pointer.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/front.hpp>
#include "APITypes.h"
#include "ConverterUtils.h"

namespace FB
{
    namespace detail { namespace properties 
    {
        template<class C, bool IsConst = false>
        struct select_get_property_functor {
            template<typename T>
            static inline 
            FB::GetPropFunctor f(C* instance, T (C::*getter)())
            { 
                return boost::bind(boost::mem_fn(getter), instance);
            }
        };
        
        template<class C>
        struct select_get_property_functor<C, /* IsConst= */ true> {
            template<typename T>
            static inline 
            FB::GetPropFunctor f(C* instance, T (C::*getter)() const)
            { 
                return boost::bind(boost::mem_fn(getter), instance);
            }
        };
                
        template<class C, typename F>
        struct getter 
        {
            enum { const_qualified =
                boost::function_types::is_member_function_pointer
                <F, boost::function_types::const_qualified>::value };

            typedef select_get_property_functor<C, const_qualified> result;
        };

        template<class C, bool IsConst = false>
        struct select_set_property_functor {
            template<typename T>
            static inline 
            FB::SetPropFunctor f(C* instance, void (C::*setter)(T))
            { 
                typedef typename FB::detail::plain_type<T>::type Ty;
                typedef FB::detail::converter<Ty, FB::variant> converter;
                return 
                boost::bind(setter, instance, 
                            boost::bind(&converter::convert, _1)); 
            }
        };
        
        template<class C>
        struct select_set_property_functor<C, /* IsConst= */ true> {
            template<typename T>
            static inline 
            void f(C* instance, void (C::*setter)(T) const, const FB::variant& v)
            { 
                typedef typename FB::detail::plain_type<T>::type Ty;
                typedef FB::detail::converter<Ty, FB::variant> converter;
                return 
                boost::bind(setter, instance, 
                            boost::bind(&converter::convert, _1)); 
            }
        };
        
        template<class C, typename F>
        struct setter 
        {
            
            enum { const_qualified =
                boost::function_types::is_member_function_pointer
                <F, boost::function_types::const_qualified>::value };

            typedef select_set_property_functor<C, const_qualified> result;
        };

        inline void dummySetter(const FB::variant&) 
        {
            
        }
    } }

    // make read/write property functor

    template<class C, typename F1, typename F2>
    inline PropertyFunctors 
    make_property(C* instance, F1 f1, F2 f2)
    {
        return PropertyFunctors(
            FB::detail::properties::getter<C, F1>::result::f(instance, f1),
            FB::detail::properties::setter<C, F2>::result::f(instance, f2));
    }

    // make read-only property

    template<class C, typename F>
    inline PropertyFunctors
    make_property(C* instance, F f)
    {
        return PropertyFunctors(
            FB::detail::properties::getter<C, F>::result::f(instance, f),
            boost::bind(FB::detail::properties::dummySetter, _1));
    }
}

#endif // PROPERTY_CONVERTER_H