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


#pragma once
#ifndef METHOD_CONVERTER_H
#define METHOD_CONVERTER_H

#if defined(_MSC_VER)
#  pragma once
#endif

#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/arithmetic/add.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/comparison/greater.hpp>
#include <boost/function.hpp>
#include "DeferredUtils.h"
#include "ConverterUtils.h"

#define _FB_MW_TPL(z, n, data) typename T##n
#define _FB_MW_Tn(z, n, data) T##n
#define _FB_MW_TLAST(n) BOOST_PP_CAT(T, BOOST_PP_SUB(n,1))

#define _FB_MW_CNLARG(n) convertArgumentSoft<typename std::decay<T##n>::type>(in, BOOST_PP_ADD(n,1))
#define _FB_MW_CLARG(n) convertLastArgument<TLast>(in, BOOST_PP_ADD(n,1))
#define _FB_MW_CARGS(z, n, t) BOOST_PP_IF(BOOST_PP_EQUAL(n, BOOST_PP_SUB(t,1)), _FB_MW_CLARG(n), _FB_MW_CNLARG(n))

#define _FB_MW_CNLARG_DFD(n) convertArgumentSoftDfd<typename std::decay<T##n>::type>(in, BOOST_PP_ADD(n,1))
#define _FB_MW_CLARG_DFD(n) convertLastArgumentDfd<TLast>(in, BOOST_PP_ADD(n,1))
#define _FB_MW_CARGS_DFD(z, n, t) BOOST_PP_IF(BOOST_PP_EQUAL(n, BOOST_PP_SUB(t,1)), _FB_MW_CLARG_DFD(n), _FB_MW_CNLARG_DFD(n))

#define _FB_MW_TLASTDEF(n) typedef typename std::decay<_FB_MW_TLAST(n)>::type TLast;

#define _FB_METHOD_WRAPPER(z, n, data)                                          \
        template<typename C, typename R                                         \
            BOOST_PP_COMMA_IF(BOOST_PP_GREATER(n,0))                            \
            BOOST_PP_ENUM(n, _FB_MW_TPL, 0), typename F>                        \
        struct method_wrapper##n                                                \
        {                                                                       \
            using result_type = FB::variantPromise;                             \
            F f;                                                                \
            method_wrapper##n(F f) : f(f) {}                                    \
            result_type operator()(C* instance, const FB::VariantList& in)      \
            {                                                                   \
                BOOST_PP_IF(BOOST_PP_GREATER(n,0), _FB_MW_TLASTDEF(n), BOOST_PP_EMPTY()) \
                /* If any of the convert_casts return a FB::Promise<T> this */  \
                /* will convert them to FB::variantPromises */                  \
                FB::VariantPromiseList args{                                    \
                    BOOST_PP_ENUM(n, _FB_MW_CARGS_DFD, n)                       \
                };                                                              \
                auto max = in.size();                                           \
                if (max > n) {                                                  \
                    for (decltype(max) i = n; i < max; ++i) {                   \
                        args.emplace_back(convertArgumentSoftDfd<FB::variant>(in, i+1));\
                    }                                                           \
                }                                                               \
                auto fn(f);                                                     \
                auto cb = [fn,instance](const FB::VariantList&in) -> FB::variantPromise { \
                    return FB::detail::convertToVariantPromise<R>((instance->*fn)(\
                        BOOST_PP_ENUM(n, _FB_MW_CARGS, n)                       \
                        ));                                                     \
                };                                                              \
                /* This will call cb with a VariantList of resolved things   */ \
                return FB::whenAllPromises(args, cb);                           \
            }                                                                   \
        };                                                                      \
        template<typename C                                                     \
            BOOST_PP_COMMA_IF(BOOST_PP_GREATER(n,0))                            \
            BOOST_PP_ENUM(n, _FB_MW_TPL, 0), typename F>                        \
        struct method_wrapper##n<C, void                                        \
            BOOST_PP_COMMA_IF(BOOST_PP_GREATER(n,0))                            \
            BOOST_PP_ENUM(n, _FB_MW_Tn, 0), F>                                  \
        {                                                                       \
            using result_type = FB::variantPromise;                             \
            F f;                                                                \
            method_wrapper##n(F f) : f(f) {}                                    \
            result_type operator()(C* instance, const FB::VariantList& in)      \
            {                                                                   \
                BOOST_PP_IF(BOOST_PP_GREATER(n,0), _FB_MW_TLASTDEF(n), BOOST_PP_EMPTY()) \
                /* If any of the convert_casts return a FB::Promise<T> this */  \
                /* will convert them to FB::variantPromises */                  \
                FB::VariantPromiseList args{                                    \
                    BOOST_PP_ENUM(n, _FB_MW_CARGS_DFD, n)                       \
                };                                                              \
                auto max = in.size();                                           \
                if (max > n) {                                            \
                    for (decltype(max) i = n; i < max; ++i) {                   \
                        args.emplace_back(convertArgumentSoftDfd<FB::variant>(in, i+1));\
                    }                                                           \
                }                                                               \
                auto fn(f);                                                     \
                auto cb = [fn,instance](const FB::VariantList&in) -> FB::variantPromise { \
                    (instance->*fn)(                                            \
                        BOOST_PP_ENUM(n, _FB_MW_CARGS, n)                       \
                        );                                                      \
                    return variant();                                           \
                };                                                              \
                /* This will call cb with a VariantList of resolved things   */ \
                return FB::whenAllPromises(args, cb);                           \
            }                                                                   \
        };

#define _FB_MAKE_METHOD(z, n, data)                                             \
    template<class C, class R                                                   \
            BOOST_PP_COMMA_IF(BOOST_PP_GREATER(n,0))                            \
            BOOST_PP_ENUM(n, _FB_MW_TPL, 0)>                                    \
    inline FB::CallMethodFunctor                                                \
    make_method(C* instance, R (C::*function)(                                  \
        BOOST_PP_ENUM(n, _FB_MW_Tn, 0)))                                        \
    {                                                                           \
        return std::bind(                                                       \
            FB::detail::methods::method_wrapper##n<C, R                         \
                BOOST_PP_COMMA_IF(BOOST_PP_GREATER(n,0))                        \
                BOOST_PP_ENUM(n, _FB_MW_Tn, 0)                                  \
                , R (C::*)(BOOST_PP_ENUM(n, _FB_MW_Tn, 0))>(function),          \
                instance, std::placeholders::_1);                               \
    }                                                                           \
    template<class C, class R                                                   \
            BOOST_PP_COMMA_IF(BOOST_PP_GREATER(n,0))                            \
            BOOST_PP_ENUM(n, _FB_MW_TPL, 0)>                                    \
    inline FB::CallMethodFunctor                                                \
    make_method(C* instance, R (C::*function)(                                  \
        BOOST_PP_ENUM(n, _FB_MW_Tn, 0)) const)                                  \
    {                                                                           \
            return std::bind(FB::detail::methods::method_wrapper##n<C, R        \
                BOOST_PP_COMMA_IF(BOOST_PP_GREATER(n,0))                        \
                BOOST_PP_ENUM(n, _FB_MW_Tn, 0)                                  \
            , R (C::*)(BOOST_PP_ENUM(n, _FB_MW_Tn, 0)) const>(function),        \
            instance, std::placeholders::_1);                                   \
    }

namespace FB
{
    namespace detail { namespace methods
    {
        using FB::convertArgumentSoft;
        using FB::convertArgumentSoftDfd;

        BOOST_PP_REPEAT(50, _FB_METHOD_WRAPPER, BOOST_PP_EMPTY())

    } } // namespace detail::methods

    BOOST_PP_REPEAT(50, _FB_MAKE_METHOD, BOOST_PP_EMPTY())

} // namespace FB

#undef _FB_METHOD_WRAPPER
#undef _FB_MAKE_METHOD
#undef _FB_MW_TPL
#undef _FB_MW_Tn
#undef _FB_MW_TLAST

#undef _FB_MW_CNLARG
#undef _FB_MW_CLARG
#undef _FB_MW_CARGS
#undef _FB_MW_TLASTDEF

#endif //METHOD_CONVERTER_H
