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

    template <typename T>
    class Promise;

    template <typename T>
    class Deferred final {
        friend class Promise < T > ;
    public:
        using type = T;
        using Callback = std::function < void(T) > ;
        using ErrCallback = std::function < void(std::exception e) > ;
        enum class State {PENDING, RESOLVED, REJECTED};

    private:
        struct StateData {
            StateData(T v) : value(v), state(State::RESOLVED) {}
            StateData() : state(State::PENDING) {}
            T value;
            State state;
            std::exception err;

            std::vector<Callback> resolveList;
            std::vector<ErrCallback> rejectList;
        };
        using StateDataPtr = std::shared_ptr < StateData > ;
        
        StateDataPtr m_data;
    public:
        Deferred(T v) : m_data(std::make_shared<StateData>(v)) {}
        Deferred() : m_data(std::make_shared<StateData>()) {}
        Deferred(Deferred<T> &&rh) : m_data(std::move(rh.m_data)) {} // Move constructor
        Deferred(const Deferred<T>& rh) : m_data(rh.m_data) {} // Copy constructor
        
        ~Deferred() { invalidate(); }

        Promise<T> promise() const {
            return Promise<T>(m_data);
        }

        void invalidate() const {
            if (m_data->state == State::PENDING) {
                reject(std::runtime_error("Deferred object destroyed"));
            }
        }

        void resolve(Promise<T> v) const {
            auto onDone = [this](T resV) {
                this->resolve(resV);
            };
            auto onFail = [this](std::exception e) { reject(e); };
            promise().done(onDone, onFail);
        }
        void resolve(T v) const {
            m_data->value = v;
            m_data->state = State::RESOLVED;
            for (auto fn : m_data->resolveList) {
                fn(v);
            }
            m_data->resolveList.clear();
        }
        void reject(std::exception e) const {
            m_data->err = e;
            m_data->state = State::REJECTED;
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

        template <typename U>
        Promise(const Promise<U>& rh) {
            Deferred<T> dfd;
            auto onDone = [dfd](U v) {
                FB::variant val{ v };
                dfd.resolve(val.convert_cast<T>());
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
                done((Deferred<T>::Callback)onDone);
                int a = 0;
                //fail((ErrCallback)onFail);
            } else {
                auto onFail = [dfd](std::exception e1) -> void {
                    dfd.reject(e1);
                };
                done(onDone);
                fail(onFail);
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
                    res.done(onDone2);
                    res.fail(onFail2);
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
                        res.done(onDone2);
                        res.fail(onFail2);
                    } catch (std::exception e2) {
                        dfd.reject(e2);
                    }
                };
                done(onDone);
                fail(onFail);
            } else {
                auto onFail = [dfd](std::exception e1) -> void {
                    dfd.reject(e1);
                };
                done(onDone);
                fail(onFail);
            }
            return dfd.promise();
        }

        const Promise<T>& done(typename Deferred<T>::Callback cbSuccess, typename Deferred<T>::ErrCallback cbFail = nullptr) const {
            if (!m_data) { throw std::runtime_error("Promise invalid"); }
            if (cbFail) { fail(cbFail); }
            if (!cbSuccess) { return *this; }
            if (m_data->state == Deferred<T>::State::PENDING) {
                m_data->resolveList.emplace_back(cbSuccess);
            } else if (m_data->state == Deferred<T>::State::RESOLVED) {
                cbSuccess(m_data->value);
            }
            return *this;
        }
        const Promise<T>& fail(typename Deferred<T>::ErrCallback cbFail) const {
            if (!m_data) { throw std::runtime_error("Promise invalid"); }
            if (!cbFail) { return *this; }
            if (m_data->state == Deferred<T>::State::PENDING) {
                m_data->rejectList.emplace_back(cbFail);
            } else if (m_data->state == Deferred<T>::State::REJECTED) {
                cbFail(m_data->err);
            }
            return *this;
        }

    };

}

#endif // H_FBDEFERRED