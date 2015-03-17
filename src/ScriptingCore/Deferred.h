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

namespace FB {

    enum class PromiseState {PENDING, RESOLVED, REJECTED};

    template <typename T>
    class Promise;
    template <>
    class Promise < void >;

    template <typename T>
    class Deferred final {
        friend class Promise < T > ;
    public:
        using type = T;
        using Callback = std::function < void(T) > ;
        using ErrCallback = std::function < void(std::exception e) > ;

    private:
        struct StateData {
            StateData(T v) : value(v), state(PromiseState::RESOLVED) {}
            StateData(std::exception e) : err(e), state(PromiseState::REJECTED) {}
            StateData() : state(PromiseState::PENDING) {}
            ~StateData() {
                if (state == PromiseState::PENDING) {
                    reject(std::runtime_error("Deferred object destroyed"));
                }
            }
            void resolve(T v) {
                value = v;
                state = PromiseState::RESOLVED;
                rejectList.clear();
                for (auto fn : resolveList) {
                    fn(v);
                }
                resolveList.clear();
            }
            void reject(std::exception e) {
                err = e;
                state = PromiseState::REJECTED;
                resolveList.clear();
                for (auto fn : rejectList) {
                    fn(e);
                }
                rejectList.clear();
            }
            T value;
            PromiseState state;
            std::exception err;

            std::vector<Callback> resolveList;
            std::vector<ErrCallback> rejectList;
        };
        using StateDataPtr = std::shared_ptr < StateData > ;
        
        StateDataPtr m_data;
    public:
        Deferred(T v) : m_data(std::make_shared<StateData>(v)) {}
        Deferred(std::exception e) : m_data(std::make_shared<StateData>(e)) {}
        Deferred() : m_data(std::make_shared<StateData>()) {}
        Deferred(Deferred<T> &&rh) : m_data(std::move(rh.m_data)) {} // Move constructor
        Deferred(const Deferred<T>& rh) : m_data(rh.m_data) {} // Copy constructor
        
        ~Deferred() { }

        Promise<T> promise() const {
            return Promise<T>(m_data);
        }

        void invalidate() const {
            if (m_data->state == PromiseState::PENDING) {
                reject(std::runtime_error("Deferred object destroyed"));
            }
        }

        void resolve(T v) const {
            m_data->resolve(v);
        }
        void resolve(Promise<T> v) const {
            Deferred<T> dfd(*this);
            auto onDone = [dfd](T resV) {
                dfd.resolve(resV);
            };
            auto onFail = [dfd](std::exception e) { dfd.reject(e); };
            v.done(onDone, onFail);
        }
        void reject(std::exception e) const {
            m_data->reject(e);
        }

    };

    template <>
    class Deferred < void > final
    {
        friend class Promise < void > ;
    public:
        using type = void;
        using Callback = std::function < void() > ;
        using ErrCallback = std::function < void(std::exception e) > ;

    private:
        struct StateData {
            StateData(bool v) {
                state = v ? PromiseState::RESOLVED : PromiseState::PENDING;
            }
            StateData(std::exception e) : err(e), state(PromiseState::REJECTED) {}
            StateData() : state(PromiseState::PENDING) {}
            PromiseState state;
            std::exception err;

            std::vector<Callback> resolveList;
            std::vector<ErrCallback> rejectList;
        };
        using StateDataPtr = std::shared_ptr < StateData > ;
        
        StateDataPtr m_data;
    public:
        Deferred(bool v) : m_data(std::make_shared<StateData>(v)) {}
        Deferred(std::exception e) : m_data(std::make_shared<StateData>(e)) {}
        Deferred() : m_data(std::make_shared<StateData>()) {}
        Deferred(Deferred<void> &&rh) : m_data(std::move(rh.m_data)) {} // Move constructor
        Deferred(const Deferred<void>& rh) : m_data(rh.m_data) {} // Copy constructor
        
        ~Deferred() { invalidate(); }

        Promise<void> promise() const;

        void invalidate() const;

        void resolve(Promise<void> v) const;
        void resolve() const {
            m_data->state = PromiseState::RESOLVED;
            for (auto fn : m_data->resolveList) {
                fn();
            }
            m_data->resolveList.clear();
        }
        void reject(std::exception e) const {
            m_data->err = e;
            m_data->state = PromiseState::REJECTED;
            for (auto fn : m_data->rejectList) {
                fn(e);
            }
            m_data->rejectList.clear();
        }

    };

    template <typename T>
    class Promise
    {
    private:
        friend class Deferred < T >;
        typename Deferred<T>::StateDataPtr m_data;

    public:
        Promise() {} // default constructor, creates invalid promise
        Promise(typename Deferred<T>::StateDataPtr data) : m_data(data) {}
        Promise(Promise&& rh) : m_data(std::move(rh.m_data)) {} // Move constructor
        Promise(const Promise<T>& rh) : m_data(rh.m_data) {} // Copy constructor
        Promise(T v) {
            Deferred<T> dfd{ v };
            m_data = dfd.promise().m_data;
        }

        Promise<T>& operator=(const Promise<T>& rh) {
            m_data = rh.m_data;
            return *this;
        }

        // available only when creating a variantDeferred, allows forcing the type in the variant.
        // Unless you *really* know what you're doing, don't use this; it's used internally. If
        // you use this it will most likely cause convert_cast to not work.
        template <typename U, typename V=T, typename std::enable_if<std::is_same<FB::variant, V>::value, int >::type = 0>
        Promise(const Promise<U>& rh, bool force) {
            Deferred<T> dfd;
            auto onDone = [dfd](U v) -> void {
                FB::variant val{ v, true };
                dfd.resolve(val);
            };
            auto onFail = [dfd](std::exception e) { dfd.reject(e); };
            rh.done(onDone, onFail);
            m_data = dfd.promise().m_data;
        }

        template <typename U, typename V=T, typename std::enable_if<std::is_same<FB::variant, V>::value, int >::type = 0>
        Promise(const Promise<U>& rh) {
            Deferred<T> dfd;
            auto onDone = [dfd](U v) -> void {
                FB::variant val{ v };
                dfd.resolve(val);
            };
            auto onFail = [dfd](std::exception e) { dfd.reject(e); };
            rh.done(onDone, onFail);
            m_data = dfd.promise().m_data;
        }

        template <typename U, typename V=T, typename std::enable_if<!std::is_same<FB::variant, V>::value, int >::type = 0>
        Promise(const Promise<U>& rh) {
            Deferred<T> dfd;
            auto onDone = [dfd](U v) -> void {
                FB::variant val{ v };
                dfd.resolve(val.convert_cast<T>());
            };
            auto onFail = [dfd](std::exception e) { dfd.reject(e); };
            rh.done(onDone, onFail);
            m_data = dfd.promise().m_data;
        }

        // Called only if U and T are not the same type (see specialization below)
        template <typename U>
        Promise<U> convert_cast() {
            return Promise<U>(*this);
        }

        template <>
        Promise<T> convert_cast() {
            return *this;
        }

        static Promise<T> rejected(std::exception e) {
            Deferred<T> dfd;
            dfd.reject(e);
            return dfd.promise();
        }

        void invalidate() {
            m_data.reset();
        }

        // piped then with static return value
        template <typename Uout>
        Promise<Uout> then(std::function<Uout(typename T)> cbSuccess, std::function<Uout(std::exception)> cbFail = nullptr) const {
            if (!m_data) { return Promise<Uout>::rejected(std::runtime_error("Promise invalid")); }
            Deferred<Uout> dfd;
            auto onDone = [dfd, cbSuccess](typename T v) -> void {
                try {
                    auto res = cbSuccess(v);
                    dfd.resolve(res);
                } catch (std::exception e) {
                    dfd.reject(e);
                }
            };
            if (cbFail) {
                auto onFail = [dfd, cbFail](std::exception e1) -> void {
                    try {
                        auto res = cbFail(e1);
                        dfd.resolve(res);
                    } catch (std::exception e2) {
                        dfd.reject(e2);
                    }
                };
                done(onDone, onFail);
            } else {
                auto onFail = [dfd](std::exception e1) -> void {
                    dfd.reject(e1);
                };
                done(onDone, onFail);
            }
            return dfd.promise();
        }

        template <>
        Promise<void> then<void>(std::function<void(typename T)> cbSuccess, std::function<void(std::exception)> cbFail) const {
            if (!m_data) { return Promise<void>::rejected(std::runtime_error("Promise invalid")); }
            Deferred<void> dfd;
            auto onDone = [dfd, cbSuccess](typename T v) -> void {
                try {
                    cbSuccess(v);
                    dfd.resolve();
                } catch (std::exception e) {
                    dfd.reject(e);
                }
            };
            if (cbFail) {
                auto onFail = [dfd, cbFail](std::exception e1) -> void {
                    try {
                        cbFail(e1);
                        dfd.resolve();
                    } catch (std::exception e2) {
                        dfd.reject(e2);
                    }
                };
                done(onDone, onFail);
            } else {
                auto onFail = [dfd](std::exception e1) -> void {
                    dfd.reject(e1);
                };
                done(onDone, onFail);
            }
            return dfd.promise();
        }

        // piped then with Deferred return value
        template <typename Uout>
        Promise<Uout> thenPipe(std::function<Promise<Uout>(typename T)> cbSuccess, std::function<Promise<Uout>(std::exception)> cbFail = nullptr) const {
            if (!m_data) { return Promise<Uout>::rejected(std::runtime_error("Promise invalid")); }
            Deferred<Uout> dfd;
            auto onDone = [dfd, cbSuccess](typename T v) -> void {
                try {
                    auto res = cbSuccess(v);
                    auto onDone2 = [dfd](Uout v) { dfd.resolve(v); };
                    auto onFail2 = [dfd](std::exception e) { dfd.reject(e); };
                    res.done(onDone2, onFail2);
                } catch (std::exception e) {
                    dfd.reject(e);
                }
            };

            if (cbFail) {
                auto onFail = [dfd, cbFail](std::exception e1) -> void {
                    try {
                        auto res = cbFail(e1);
                        auto onDone2 = [dfd](Uout v) { dfd.resolve(v); };
                        auto onFail2 = [dfd](std::exception e) { dfd.reject(e); };
                        res.done(onDone2, onFail2);
                    } catch (std::exception e2) {
                        dfd.reject(e2);
                    }
                };
                done(onDone, onFail);
            } else {
                auto onFail = [dfd](std::exception e1) -> void {
                    dfd.reject(e1);
                };
                done(onDone, onFail);
            }
            return dfd.promise();
        }

        const Promise<T>& done(typename Deferred<T>::Callback cbSuccess, typename Deferred<T>::ErrCallback cbFail = nullptr) const {
            if (!m_data) { throw std::runtime_error("Promise invalid"); }
            if (cbFail) { fail(cbFail); }
            if (!cbSuccess) { return *this; }
            if (m_data->state == PromiseState::PENDING) {
                m_data->resolveList.emplace_back(cbSuccess);
            } else if (m_data->state == PromiseState::RESOLVED) {
                cbSuccess(m_data->value);
            }
            return *this;
        }
        const Promise<T>& fail(typename Deferred<T>::ErrCallback cbFail) const {
            if (!m_data) { throw std::runtime_error("Promise invalid"); }
            if (!cbFail) { return *this; }
            if (m_data->state == PromiseState::PENDING) {
                m_data->rejectList.emplace_back(cbFail);
            } else if (m_data->state == PromiseState::REJECTED) {
                cbFail(m_data->err);
            }
            return *this;
        }

    };

    template <> // specialization for Promise<void>
    class Promise<void>
    {
    private:
        friend class Deferred < void >;
        Deferred<void>::StateDataPtr m_data;

    public:
        Promise() {} // default constructor, creates invalid promise
        Promise(Deferred<void>::StateDataPtr data) : m_data(data) {}
        Promise(Promise&& rh) : m_data(std::move(rh.m_data)) {} // Move constructor
        Promise(const Promise& rh) : m_data(rh.m_data) {} // Copy constructor
        Promise(bool v) {
            Deferred<void> dfd{ v };
            m_data = dfd.promise().m_data;
        }

        Promise<void>& operator=(const Promise<void>& rh) {
            m_data = rh.m_data;
            return *this;
        }

        template <typename U>
        Promise(const Promise<U>& rh) {
            Deferred<T> dfd;
            auto onDone = [dfd](U v) {
                dfd.resolve();
            };
            auto onFail = [dfd](std::exception e) { dfd.reject(e); };
            done(onDone, onFail);
            m_data = dfd.promise().m_data;
        }

        // Called only if U and T are not the same type (see specialization below)
        template <typename U>
        Promise<U> convert_cast() {
            return Promise<U>(*this);
        }

        template <>
        Promise<void> convert_cast() {
            return *this;
        }

        static Promise<void> rejected(std::exception e) {
            Deferred<void> dfd;
            dfd.reject(e);
            return dfd.promise();
        }

        void invalidate() {
            m_data.reset();
        }

        // piped then with static return value
        template <typename Uout>
        Promise<Uout> then(std::function<Uout()> cbSuccess, std::function<Uout(std::exception)> cbFail = nullptr) const {
            if (!m_data) { return Promise<Uout>::rejected(std::runtime_error("Promise invalid")); }
            Deferred<Uout> dfd;
            auto onDone = [dfd, cbSuccess]() -> void {
                try {
                    auto res = cbSuccess();
                    dfd.resolve(res);
                } catch (std::exception e) {
                    dfd.reject(e);
                }
            };
            if (cbFail) {
                auto onFail = [dfd, cbFail](std::exception e1) -> void {
                    try {
                        auto res = cbFail(e1);
                        dfd.resolve(res);
                    } catch (std::exception e2) {
                        dfd.reject(e2);
                    }
                };
                done(onDone, onFail);
            } else {
                auto onFail = [dfd](std::exception e1) -> void {
                    dfd.reject(e1);
                };
                done(onDone, onFail);
            }
            return dfd.promise();
        }

        // piped then with Deferred return value
        template <typename Uout>
        Promise<Uout> thenPipe(std::function<Promise<Uout>()> cbSuccess, std::function<Promise<Uout>(std::exception)> cbFail = nullptr) const {
            if (!m_data) { return Promise<Uout>::rejected(std::runtime_error("Promise invalid")); }
            Deferred<Uout> dfd;
            auto onDone = [dfd, cbSuccess]() -> void {
                try {
                    auto res = cbSuccess();
                    auto onDone2 = [dfd](Uout v) { dfd.resolve(v); };
                    auto onFail2 = [dfd](std::exception e) { dfd.reject(e); };
                    res.done(onDone2, onFail2);
                } catch (std::exception e) {
                    dfd.reject(e);
                }
            };

            if (cbFail) {
                auto onFail = [dfd, cbFail](std::exception e1) -> void {
                    try {
                        auto res = cbFail(e1);
                        auto onDone2 = [dfd](Uout v) { dfd.resolve(v); };
                        auto onFail2 = [dfd](std::exception e) { dfd.reject(e); };
                        res.done(onDone2, onFail2);
                    } catch (std::exception e2) {
                        dfd.reject(e2);
                    }
                };
                done(onDone, onFail);
            } else {
                auto onFail = [dfd](std::exception e1) -> void {
                    dfd.reject(e1);
                };
                done(onDone, onFail);
            }
            return dfd.promise();
        }

        const Promise<void>& done(Deferred<void>::Callback cbSuccess, Deferred<void>::ErrCallback cbFail = nullptr) const {
            if (!m_data) { throw std::runtime_error("Promise invalid"); }
            if (cbFail) { fail(cbFail); }
            if (!cbSuccess) { return *this; }
            if (m_data->state == PromiseState::PENDING) {
                m_data->resolveList.emplace_back(cbSuccess);
            } else if (m_data->state == PromiseState::RESOLVED) {
                cbSuccess();
            }
            return *this;
        }
        const Promise<void>& fail(Deferred<void>::ErrCallback cbFail) const {
            if (!m_data) { throw std::runtime_error("Promise invalid"); }
            if (!cbFail) { return *this; }
            if (m_data->state == PromiseState::PENDING) {
                m_data->rejectList.emplace_back(cbFail);
            } else if (m_data->state == PromiseState::REJECTED) {
                cbFail(m_data->err);
            }
            return *this;
        }

    };

}

#endif // H_FBDEFERRED