/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Feb 10, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2015 Richard Bateman and the FireBreath Dev Team
\**********************************************************/

#pragma once
#ifndef H_FBDEFERRED
#define H_FBDEFERRED

#include <functional>
#include <type_traits>
#include <APITypes.h>
#include <boost/noncopyable.hpp>

namespace FB {
    template <typename T>
    DeferredPtr<T> makeDeferred() {
        return std::make_shared<Deferred<T>>(Deferred<T>::PrivateOnly());
    }

    template <typename T>
    DeferredPtr<T> makeDeferred(T v) {
        return std::make_shared<Deferred<T>>(v, Deferred<T>::PrivateOnly());
    }

    template <typename T>
    struct is_Deferred : std::false_type { };
    template <typename T>
    struct is_Deferred<DeferredPtr<T> > : std::true_type {}; 

    template <typename T>
    class Deferred final : public std::enable_shared_from_this<Deferred<T>>, boost::noncopyable {
    public:
        using type = T;
        using Callback = std::function < void(T) > ;
        using ErrCallback = std::function < void(std::exception e) > ;
        enum class State {PENDING, RESOLVED, REJECTED};

    private:
        struct PrivateOnly {};
    public:
        Deferred(T v, PrivateOnly) : m_value(v), m_state(State::RESOLVED) { }
        Deferred(PrivateOnly) : m_state(State::PENDING) {}
        
        ~Deferred() { invalidate(); }

        void invalidate() {
            if (m_state == State::PENDING) {
                reject(std::runtime_error("Deferred object destroyed"));
            }
        }

        // Called only if U and T are not the same type (see specialization below)
        template <typename U>
        DeferredPtr<U> convert_cast() {
            DeferredPtr<U> dfdOut = makeDeferred<U>();
            auto onDone = [dfdOut](T v) {
                FB::variant val{ v };
                dfdOut->resolve(val.convert_cast<U>());
            };
            auto onFail = [dfdOut](std::exception e) { dfdOut->reject(e); };
            done(onDone);
            fail(onFail);
            return dfdOut;
        }

        template <>
        DeferredPtr<T> convert_cast() {
            return shared_from_this();
        }

        // piped then with static return value
        template <typename Uout>
        DeferredPtr<Uout> then(std::function<Uout(typename T)> cbSuccess, std::function<Uout(std::exception)> cbFail = nullptr)
        {
            DeferredPtr<Uout> dfdOut = makeDeferred<Uout>();
            auto onDone = [dfdOut,cbSuccess](typename T v) -> void {
                try {
                    auto res = cbSuccess(v);
                    dfdOut->resolve(res);
                } catch (std::exception e) {
                    dfdOut->reject(e);
                }
            };
            if (cbFail) {
                auto onFail = [dfdOut,cbFail](std::exception e1) -> void {
                    try {
                        auto res = cbFail(e1);
                        dfdOut->resolve(res);
                    } catch (std::exception e2) {
                        dfdOut->reject(e2);
                    }
                };
                done((Callback)onDone);
                int a = 0;
                //fail((ErrCallback)onFail);
            } else {
                auto onFail = [dfdOut](std::exception e1) -> void {
                    dfdOut->reject(e1);
                };
                done(onDone);
                fail(onFail);
            }
            return dfdOut;
        }

        // piped then with Deferred return value
        template <typename Uout>
        DeferredPtr<Uout> thenPipe(std::function<DeferredPtr<Uout>(typename T)> cbSuccess, std::function<DeferredPtr<Uout>(std::exception)> cbFail = nullptr)
        {
            DeferredPtr<Uout> dfdOut = makeDeferred<Uout>();
            auto onDone = [dfdOut,cbSuccess](typename T v) -> void {
                try {
                    auto res = cbSuccess(v);
                    auto onDone2 = [dfdOut](Uout v) { dfdOut->resolve(v); };
                    auto onFail2 = [dfdOut](std::exception e) { dfdOut->reject(e); };
                    res->done(onDone2);
                    res->fail(onFail2);
                } catch (std::exception e) {
                    dfdOut->reject(e);
                }
            };

            if (cbFail) {
                auto onFail = [dfdOut,cbFail](std::exception e1) -> void {
                    try {
                        auto res = cbFail(e1);
                        auto onDone2 = [dfdOut](Uout v) { dfdOut->resolve(v); };
                        auto onFail2 = [dfdOut](std::exception e) { dfdOut->reject(e); };
                        res->done(onDone2);
                        res->fail(onFail2);
                    } catch (std::exception e2) {
                        dfdOut->reject(e2);
                    }
                };
                done(onDone);
                fail(onFail);
            } else {
                auto onFail = [dfdOut](std::exception e1) -> void {
                    dfdOut->reject(e1);
                };
                done(onDone);
                fail(onFail);
            }
            return dfdOut;
        }

        void done(Callback cbSuccess) {
            if (!cbSuccess) { return; }
            if (m_state == State::PENDING) {
                m_onResolve.emplace_back(cbSuccess);
            } else if (m_state == State::RESOLVED) {
                cbSuccess(m_value);
            }
        }
        void fail(ErrCallback cbFail) {
            if (!cbFail) { return; }
            if (m_state == State::PENDING) {
                m_onReject.emplace_back(cbFail);
            } else if (m_state == State::REJECTED) {
                cbFail(m_err);
            }
        }

        void resolve(DeferredPtr<T> v) {
            auto self = shared_from_this();
            auto onDone = [self](T resV) {
                self->resolve(resV);
            };
            auto onFail = [self](std::exception e) { self->reject(e); };
            done(onDone);
            fail(onFail);
        }
        void resolve(T v) {
            m_value = v;
            m_state = State::RESOLVED;
            for (auto fn : m_onResolve) {
                fn(v);
            }
            m_onResolve.clear();
        }
        void reject(std::exception e) {
            m_err = e;
            m_state = State::REJECTED;
            for (auto fn : m_onReject) {
                fn(e);
            }
            m_onReject.clear();
        }

    private:
        T m_value;
        State m_state;
        std::exception m_err;

        std::vector<Callback> m_onResolve;
        std::vector<ErrCallback> m_onReject;

        template <typename T>
        friend DeferredPtr<T> makeDeferred(T v);

        template <typename T>
        friend DeferredPtr<T> makeDeferred();
    };

    template <typename T>
    variantDeferredPtr makeVariantDeferred(FB::DeferredPtr<T> v) {
        return v->convert_cast<FB::variant>();
    }

    variantDeferredPtr makeVariantDeferred(FB::variant v);

    variantDeferredPtr makeVariantDeferred();

}

#endif // H_FBDEFERRED