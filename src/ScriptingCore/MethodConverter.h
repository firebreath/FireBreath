/**********************************************************\
Original Author: Georg Fritzsche
Generated on: 2010-02-03
License:      Dual license model; choose one of two:
              New BSD License
              http://www.opensource.org/licenses/bsd-license.php
              - or -
              GNU Lesser General Public License, version 2.1
              http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/


#if !defined(METHOD_CONVERTER_H)
#define METHOD_CONVERTER_H

#if defined(_MSC_VER)
#  pragma once
#endif

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "ConverterUtils.h"

namespace FB
{
    namespace detail { namespace methods
    {
        using FB::detail::plain_type;
        using FB::convertArgument;
        
        template<class C, class R, class F>
        struct method_wrapper0
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper0(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                if(in.size() != 0)
                    return FB::variant();
                return (instance->*f)();
            }
        };
        template<class C, class F>
        struct method_wrapper0<C, void, F>
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper0(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                if(in.size() != 0)
                    return FB::variant();
                (instance->*f)();
                return FB::variant();
            }
        };
        template<class C, class R, class T0, class F>
        struct method_wrapper1
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper1(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T0>::type TLast;
                if(!checkArgumentCount<TLast>(in, 1))
                    return FB::variant();
                return (instance->*f)(
                    convertLastArgument<TLast>(in, 1));
            }
        };
        template<class C, class T0, class F>
        struct method_wrapper1<C, void, T0, F>
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper1(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T0>::type TLast;
                if(!checkArgumentCount<TLast>(in, 1))
                    return FB::variant();
                (instance->*f)(
                    convertLastArgument<TLast>(in, 1));
                return FB::variant();
            }
        };
        template<class C, class R, class T0, class T1, class F>
        struct method_wrapper2
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper2(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T1>::type TLast;
                if(!checkArgumentCount<TLast>(in, 2))
                    return FB::variant();
                return (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertLastArgument<TLast>(in, 2));
            }
        };
        template<class C, class T0, class T1, class F>
        struct method_wrapper2<C, void, T0, T1, F>
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper2(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T1>::type TLast;
                if(!checkArgumentCount<TLast>(in, 2))
                    return FB::variant();
                (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertLastArgument<TLast>(in, 2));
                return FB::variant();
            }
        };
        template<class C, class R, class T0, class T1, class T2, class F>
        struct method_wrapper3
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper3(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T2>::type TLast;
                if(!checkArgumentCount<TLast>(in, 3))
                    return FB::variant();
                return (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertArgument<typename plain_type<T1>::type>(in[1], 2),
                    convertLastArgument<TLast>(in, 3));
            }
        };
        template<class C, class T0, class T1, class T2, class F>
        struct method_wrapper3<C, void, T0, T1, T2, F>
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper3(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T2>::type TLast;
                if(!checkArgumentCount<TLast>(in, 3))
                    return FB::variant();
                (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertArgument<typename plain_type<T1>::type>(in[1], 2),
                    convertLastArgument<TLast>(in, 3));
                return FB::variant();
            }
        };
        template<class C, class R, class T0, class T1, class T2, class T3, class F>
        struct method_wrapper4
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper4(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T3>::type TLast;
                if(!checkArgumentCount<TLast>(in, 4))
                    return FB::variant();
                return (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertArgument<typename plain_type<T1>::type>(in[1], 2),
                    convertArgument<typename plain_type<T2>::type>(in[2], 3),
                    convertLastArgument<TLast>(in, 4));
            }
        };
        template<class C, class T0, class T1, class T2, class T3, class F>
        struct method_wrapper4<C, void, T0, T1, T2, T3, F>
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper4(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T3>::type TLast;
                if(!checkArgumentCount<TLast>(in, 4))
                    return FB::variant();
                (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertArgument<typename plain_type<T1>::type>(in[1], 2),
                    convertArgument<typename plain_type<T2>::type>(in[2], 3),
                    convertLastArgument<TLast>(in, 4));
                return FB::variant();
            }
        };
        template<class C, class R, class T0, class T1, class T2, class T3, class T4, class F>
        struct method_wrapper5
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper5(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T4>::type TLast;
                if(!checkArgumentCount<TLast>(in, 5))
                    return FB::variant();
                return (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertArgument<typename plain_type<T1>::type>(in[1], 2),
                    convertArgument<typename plain_type<T2>::type>(in[2], 3),
                    convertArgument<typename plain_type<T3>::type>(in[3], 4),
                    convertLastArgument<TLast>(in, 5));
            }
        };
        template<class C, class T0, class T1, class T2, class T3, class T4, class F>
        struct method_wrapper5<C, void, T0, T1, T2, T3, T4, F>
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper5(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T4>::type TLast;
                if(!checkArgumentCount<TLast>(in, 5))
                    return FB::variant();
                (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertArgument<typename plain_type<T1>::type>(in[1], 2),
                    convertArgument<typename plain_type<T2>::type>(in[2], 3),
                    convertArgument<typename plain_type<T3>::type>(in[3], 4),
                    convertLastArgument<TLast>(in, 5));
                return FB::variant();
            }
        };
        template<class C, class R, class T0, class T1, class T2, class T3, class T4, class T5, class F>
        struct method_wrapper6
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper6(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T5>::type TLast;
                if(!checkArgumentCount<TLast>(in, 6))
                    return FB::variant();
                return (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertArgument<typename plain_type<T1>::type>(in[1], 2),
                    convertArgument<typename plain_type<T2>::type>(in[2], 3),
                    convertArgument<typename plain_type<T3>::type>(in[3], 4),
                    convertArgument<typename plain_type<T4>::type>(in[4], 5),
                    convertLastArgument<TLast>(in, 6));
            }
        };
        template<class C, class T0, class T1, class T2, class T3, class T4, class T5, class F>
        struct method_wrapper6<C, void, T0, T1, T2, T3, T4, T5, F>
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper6(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T5>::type TLast;
                if(!checkArgumentCount<TLast>(in, 6))
                    return FB::variant();
                (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertArgument<typename plain_type<T1>::type>(in[1], 2),
                    convertArgument<typename plain_type<T2>::type>(in[2], 3),
                    convertArgument<typename plain_type<T3>::type>(in[3], 4),
                    convertArgument<typename plain_type<T4>::type>(in[4], 5),
                    convertLastArgument<TLast>(in, 6));
                return FB::variant();
            }
        };
        template<class C, class R, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class F>
        struct method_wrapper7
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper7(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T6>::type TLast;
                if(!checkArgumentCount<TLast>(in, 7))
                    return FB::variant();
                return (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertArgument<typename plain_type<T1>::type>(in[1], 2),
                    convertArgument<typename plain_type<T2>::type>(in[2], 3),
                    convertArgument<typename plain_type<T3>::type>(in[3], 4),
                    convertArgument<typename plain_type<T4>::type>(in[4], 5),
                    convertArgument<typename plain_type<T5>::type>(in[5], 6),
                    convertLastArgument<TLast>(in, 7));
            }
        };
        template<class C, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class F>
        struct method_wrapper7<C, void, T0, T1, T2, T3, T4, T5, T6, F>
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper7(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T6>::type TLast;
                if(!checkArgumentCount<TLast>(in, 7))
                    return FB::variant();
                (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertArgument<typename plain_type<T1>::type>(in[1], 2),
                    convertArgument<typename plain_type<T2>::type>(in[2], 3),
                    convertArgument<typename plain_type<T3>::type>(in[3], 4),
                    convertArgument<typename plain_type<T4>::type>(in[4], 5),
                    convertArgument<typename plain_type<T5>::type>(in[5], 6),
                    convertLastArgument<TLast>(in, 7));
                return FB::variant();
            }
        };
        template<class C, class R, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class F>
        struct method_wrapper8
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper8(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T7>::type TLast;
                if(!checkArgumentCount<TLast>(in, 8))
                    return FB::variant();
                return (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertArgument<typename plain_type<T1>::type>(in[1], 2),
                    convertArgument<typename plain_type<T2>::type>(in[2], 3),
                    convertArgument<typename plain_type<T3>::type>(in[3], 4),
                    convertArgument<typename plain_type<T4>::type>(in[4], 5),
                    convertArgument<typename plain_type<T5>::type>(in[5], 6),
                    convertArgument<typename plain_type<T6>::type>(in[6], 7),
                    convertLastArgument<TLast>(in, 8));
            }
        };
        template<class C, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class F>
        struct method_wrapper8<C, void, T0, T1, T2, T3, T4, T5, T6, T7, F>
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper8(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T7>::type TLast;
                if(!checkArgumentCount<TLast>(in, 8))
                    return FB::variant();
                (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertArgument<typename plain_type<T1>::type>(in[1], 2),
                    convertArgument<typename plain_type<T2>::type>(in[2], 3),
                    convertArgument<typename plain_type<T3>::type>(in[3], 4),
                    convertArgument<typename plain_type<T4>::type>(in[4], 5),
                    convertArgument<typename plain_type<T5>::type>(in[5], 6),
                    convertArgument<typename plain_type<T6>::type>(in[6], 7),
                    convertLastArgument<TLast>(in, 8));
                return FB::variant();
            }
        };
        template<class C, class R, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class F>
        struct method_wrapper9
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper9(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T8>::type TLast;
                if(!checkArgumentCount<TLast>(in, 9))
                    return FB::variant();
                return (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertArgument<typename plain_type<T1>::type>(in[1], 2),
                    convertArgument<typename plain_type<T2>::type>(in[2], 3),
                    convertArgument<typename plain_type<T3>::type>(in[3], 4),
                    convertArgument<typename plain_type<T4>::type>(in[4], 5),
                    convertArgument<typename plain_type<T5>::type>(in[5], 6),
                    convertArgument<typename plain_type<T6>::type>(in[6], 7),
                    convertArgument<typename plain_type<T7>::type>(in[7], 8),
                    convertLastArgument<TLast>(in, 9));
            }
        };
        template<class C, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class F>
        struct method_wrapper9<C, void, T0, T1, T2, T3, T4, T5, T6, T7, T8, F>
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper9(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T8>::type TLast;
                if(!checkArgumentCount<TLast>(in, 9))
                    return FB::variant();
                (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertArgument<typename plain_type<T1>::type>(in[1], 2),
                    convertArgument<typename plain_type<T2>::type>(in[2], 3),
                    convertArgument<typename plain_type<T3>::type>(in[3], 4),
                    convertArgument<typename plain_type<T4>::type>(in[4], 5),
                    convertArgument<typename plain_type<T5>::type>(in[5], 6),
                    convertArgument<typename plain_type<T6>::type>(in[6], 7),
                    convertArgument<typename plain_type<T7>::type>(in[7], 8),
                    convertLastArgument<TLast>(in, 9));
                return FB::variant();
            }
        };
        template<class C, class R, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class F>
        struct method_wrapper10
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper10(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T9>::type TLast;
                if(!checkArgumentCount<TLast>(in, 10))
                    return FB::variant();
                return (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertArgument<typename plain_type<T1>::type>(in[1], 2),
                    convertArgument<typename plain_type<T2>::type>(in[2], 3),
                    convertArgument<typename plain_type<T3>::type>(in[3], 4),
                    convertArgument<typename plain_type<T4>::type>(in[4], 5),
                    convertArgument<typename plain_type<T5>::type>(in[5], 6),
                    convertArgument<typename plain_type<T6>::type>(in[6], 7),
                    convertArgument<typename plain_type<T7>::type>(in[7], 8),
                    convertArgument<typename plain_type<T8>::type>(in[8], 9),
                    convertLastArgument<TLast>(in, 10));
            }
        };
        template<class C, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class F>
        struct method_wrapper10<C, void, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, F>
        {
            typedef FB::variant result_type;
            F f;
            method_wrapper10(F f) : f(f) {}
            FB::variant operator()(C* instance, const FB::VariantList& in)
            {
                typedef typename plain_type<T9>::type TLast;
                if(!checkArgumentCount<TLast>(in, 10))
                    return FB::variant();
                (instance->*f)(
                    convertArgument<typename plain_type<T0>::type>(in[0], 1),
                    convertArgument<typename plain_type<T1>::type>(in[1], 2),
                    convertArgument<typename plain_type<T2>::type>(in[2], 3),
                    convertArgument<typename plain_type<T3>::type>(in[3], 4),
                    convertArgument<typename plain_type<T4>::type>(in[4], 5),
                    convertArgument<typename plain_type<T5>::type>(in[5], 6),
                    convertArgument<typename plain_type<T6>::type>(in[6], 7),
                    convertArgument<typename plain_type<T7>::type>(in[7], 8),
                    convertArgument<typename plain_type<T8>::type>(in[8], 9),
                    convertLastArgument<TLast>(in, 10));
                return FB::variant();
            }
        };
    } } // namespace detail::methods
    
    template<class C, class R>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)())
    {
        return boost::bind(FB::detail::methods::method_wrapper0<C, R, R (C::*)()>(function), instance, _1);
    }
    
    template<class C, class R>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)() const)
    {
        return boost::bind(FB::detail::methods::method_wrapper0<C, R, R (C::*)() const>(function), instance, _1);
    }
    
    template<class C, class R, class T0>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0))
    {
        return boost::bind(FB::detail::methods::method_wrapper1<C, R, T0, R (C::*)(T0)>(function), instance, _1);
    }
    
    template<class C, class R, class T0>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0) const)
    {
        return boost::bind(FB::detail::methods::method_wrapper1<C, R, T0, R (C::*)(T0) const>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1))
    {
        return boost::bind(FB::detail::methods::method_wrapper2<C, R, T0, T1, R (C::*)(T0, T1)>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1) const)
    {
        return boost::bind(FB::detail::methods::method_wrapper2<C, R, T0, T1, R (C::*)(T0, T1) const>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1, class T2>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1, T2))
    {
        return boost::bind(FB::detail::methods::method_wrapper3<C, R, T0, T1, T2, R (C::*)(T0, T1, T2)>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1, class T2>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1, T2) const)
    {
        return boost::bind(FB::detail::methods::method_wrapper3<C, R, T0, T1, T2, R (C::*)(T0, T1, T2) const>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1, class T2, class T3>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1, T2, T3))
    {
        return boost::bind(FB::detail::methods::method_wrapper4<C, R, T0, T1, T2, T3, R (C::*)(T0, T1, T2, T3)>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1, class T2, class T3>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1, T2, T3) const)
    {
        return boost::bind(FB::detail::methods::method_wrapper4<C, R, T0, T1, T2, T3, R (C::*)(T0, T1, T2, T3) const>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1, class T2, class T3, class T4>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4))
    {
        return boost::bind(FB::detail::methods::method_wrapper5<C, R, T0, T1, T2, T3, T4, R (C::*)(T0, T1, T2, T3, T4)>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1, class T2, class T3, class T4>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4) const)
    {
        return boost::bind(FB::detail::methods::method_wrapper5<C, R, T0, T1, T2, T3, T4, R (C::*)(T0, T1, T2, T3, T4) const>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1, class T2, class T3, class T4, class T5>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4, T5))
    {
        return boost::bind(FB::detail::methods::method_wrapper6<C, R, T0, T1, T2, T3, T4, T5, R (C::*)(T0, T1, T2, T3, T4, T5)>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1, class T2, class T3, class T4, class T5>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4, T5) const)
    {
        return boost::bind(FB::detail::methods::method_wrapper6<C, R, T0, T1, T2, T3, T4, T5, R (C::*)(T0, T1, T2, T3, T4, T5) const>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4, T5, T6))
    {
        return boost::bind(FB::detail::methods::method_wrapper7<C, R, T0, T1, T2, T3, T4, T5, T6, R (C::*)(T0, T1, T2, T3, T4, T5, T6)>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4, T5, T6) const)
    {
        return boost::bind(FB::detail::methods::method_wrapper7<C, R, T0, T1, T2, T3, T4, T5, T6, R (C::*)(T0, T1, T2, T3, T4, T5, T6) const>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4, T5, T6, T7))
    {
        return boost::bind(FB::detail::methods::method_wrapper8<C, R, T0, T1, T2, T3, T4, T5, T6, T7, R (C::*)(T0, T1, T2, T3, T4, T5, T6, T7)>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4, T5, T6, T7) const)
    {
        return boost::bind(FB::detail::methods::method_wrapper8<C, R, T0, T1, T2, T3, T4, T5, T6, T7, R (C::*)(T0, T1, T2, T3, T4, T5, T6, T7) const>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4, T5, T6, T7, T8))
    {
        return boost::bind(FB::detail::methods::method_wrapper9<C, R, T0, T1, T2, T3, T4, T5, T6, T7, T8, R (C::*)(T0, T1, T2, T3, T4, T5, T6, T7, T8)>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4, T5, T6, T7, T8) const)
    {
        return boost::bind(FB::detail::methods::method_wrapper9<C, R, T0, T1, T2, T3, T4, T5, T6, T7, T8, R (C::*)(T0, T1, T2, T3, T4, T5, T6, T7, T8) const>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9))
    {
        return boost::bind(FB::detail::methods::method_wrapper10<C, R, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, R (C::*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9)>(function), instance, _1);
    }
    
    template<class C, class R, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    inline FB::CallMethodFunctor
    make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) const)
    {
        return boost::bind(FB::detail::methods::method_wrapper10<C, R, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, R (C::*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) const>(function), instance, _1);
    }
    
} // namespace FB

#endif //METHOD_CONVERTER_H

