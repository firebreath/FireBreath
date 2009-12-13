/**********************************************************\
Original Author: Georg Fritzsche
Created: 2009-11-07
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
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
        template<class C, typename F>
		struct getter 
		{
			template<bool IsConst = false>
			struct helper {
				template<typename T>
				static inline 
				FB::GetPropFunctor f(C* instance, T (C::*getter)())
				{ 
					return boost::bind(getter, instance);
				}
			};

			template<>
			struct helper</* IsConst= */ true> {
				template<typename T>
				static inline 
				FB::GetPropFunctor f(C* instance, T (C::*getter)() const)
				{ 
					return boost::bind(getter, instance);
				}
			};

			enum { const_qualified =
				boost::function_types::is_member_function_pointer
				<F, boost::function_types::const_qualified>::value };

		    typedef helper<const_qualified> result;
		};

		template<class C, typename F>
		struct setter 
		{
			template<bool IsConst = false>
			struct helper {
				template<typename T>
				static inline 
				FB::SetPropFunctor f(C* instance, void (C::*setter)(T))
				{ 
					typedef typename FB::detail::plain_type<T>::type Ty;
					return boost::bind(setter, instance, boost::bind(&FB::detail::converter<Ty, FB::variant>::convert, _1)); 
				}
			};

			template<>
			struct helper</* IsConst= */ true> {
				template<typename T>
				static inline 
				void f(C* instance, void (C::*setter)(T) const, const FB::variant& v)
				{ 
					typedef typename FB::detail::plain_type<T>::type Ty;
					return boost::bind(setter, instance, boost::bind(&FB::detail::converter<Ty, FB::variant>::convert, _1)); 
				}
			};

			enum { const_qualified =
				boost::function_types::is_member_function_pointer
				<F, boost::function_types::const_qualified>::value };
			typedef typename boost::function_types::parameter_types<F> parameters;
			typedef typename boost::mpl::front<parameters>::type param_type;
		    typedef helper<const_qualified> result;
		};

#if 0
        template<class C, typename T>
        inline void setter(C* instance, void (C::*setter)(T), const FB::variant& v)
        {
            typedef typename FB::detail::plain_type<T>::type Ty;
            (instance->*setter)(FB::convertArgument<Ty>(v));
        }
#endif

        inline void dummySetter(const FB::variant&) 
        {
            
        }
    } }

    // make read/write property functor

#if 0
    template<class C, typename T1, typename T2>
    inline PropertyFunctors 
    make_property(C* instance, T1 (C::*getter)(), void (C::*setter)(T2))
    {
        typedef typename FB::detail::plain_type<T1>::type _T1;
        typedef typename FB::detail::plain_type<T2>::type _T2;
        enum { equal_types = boost::mpl::equal<_T1, _T2>::type::value };
        BOOST_MPL_ASSERT_MSG(equal_types,
                             PROPERTY_SETTER_AND_GETTER_TYPES_HAVE_TO_BE_EQUAL,
                             (_T1, _T2));

        return PropertyFunctors(
            boost::bind(FB::detail::properties::getter<C, T1>, instance, getter),
            boost::bind(FB::detail::properties::setter<C, T2>, instance, setter, _1));
    }

	template<class C, typename T1, typename T2>
    inline PropertyFunctors 
    make_property(C* instance, T1 (C::*getter)() const, void (C::*setter)(T2))
    {
        typedef typename FB::detail::plain_type<T1>::type _T1;
        typedef typename FB::detail::plain_type<T2>::type _T2;
        enum { equal_types = boost::mpl::equal<_T1, _T2>::type::value };
        BOOST_MPL_ASSERT_MSG(equal_types,
                             PROPERTY_SETTER_AND_GETTER_TYPES_HAVE_TO_BE_EQUAL,
                             (_T1, _T2));

        return PropertyFunctors(
            boost::bind(FB::detail::properties::getter<C, T1>, instance, getter),
            boost::bind(FB::detail::properties::setter<C, T2>, instance, setter, _1));
    }
#endif

	template<class C, typename F1, typename F2>
    inline PropertyFunctors 
    make_property(C* instance, F1 f1, F2 f2)
    {
        return PropertyFunctors(
			FB::detail::properties::getter<C, F1>::result::f(instance, f1),
			FB::detail::properties::setter<C, F2>::result::f(instance, f2));
    }

    // make read-only property

#if 0
    template<class C, typename T>
    inline PropertyFunctors
    make_property(C* instance, T (C::*getter)())
    {
        typedef typename FB::detail::plain_type<T>::type _T;

        return PropertyFunctors(
            boost::bind(FB::detail::properties::getter<C, T>, instance, getter),
            boost::bind(FB::detail::properties::dummySetter, _1));
    }
#endif

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