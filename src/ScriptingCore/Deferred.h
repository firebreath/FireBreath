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
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief  Asynchronous return value which will reject or resolve to a value of
    /// type T
    ///
    /// FB::Promise objects are patterned after the Javascript Promise pattern and are a method for
    /// returning a value which will *eventually* resolve to a value, or reject with an exception
    /// in case of an error.  To create a FB::Promise object, first create a FB::Deferred<T> which
    /// allows you to control it and then call FB::Deferred::promise()
    ///
    /// They can be chained to other FB::Promise objects and are extremely useful.
    ///
    /// FB::Promise objects can be moved or copied; all copies share state, are controlled by the same FB::Deferred object(s), and thus resolve or reject together.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename T> 
    class Promise;
    template <> 
    class Promise<void>;
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief  Resolves or Rejects a Promise object, used to create a new Promise
    ///
    /// FB::Deferred objects are only useful as the control mechanism for a FB::Promise object.
    /// To create a new Promise, first create a FB::Deferred and then return the promise for it. Retain
    /// a reference to the Deferred or to a copy of it and call resolve or reject when the result of the promise is known.
    ///
    /// A FB::Deferred object can be moved or copied; all copies share state and retain control over any associated FB::Promise objects.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename T> 
    class Deferred final { 
        friend class Promise<T>;
    public: 
        using type = T;
        using Callback = std::function<void(T)>;
        using ErrCallback = std::function<void(std::exception_ptr ep)>;
        
    private:
        struct StateData {
            StateData(T v) : value(v), state(PromiseState::RESOLVED) {}
            StateData(std::exception_ptr ep) : state(PromiseState::REJECTED), err_ptr(ep) {}
            StateData() : state(PromiseState::PENDING) {}
            ~StateData() {
                if (state == PromiseState::PENDING && rejectList.size()) {
                    reject(std::make_exception_ptr(std::runtime_error("Deferred object destroyed: 1")));
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
            void reject(std::exception_ptr ep) {
                err_ptr = ep;
                state = PromiseState::REJECTED;
                resolveList.clear();
                for (auto fn : rejectList) {
                    fn(ep);
                }
                rejectList.clear();
            }
            T value;
            PromiseState state;
            std::exception_ptr err_ptr;
            
            std::vector<Callback> resolveList;
            std::vector<ErrCallback> rejectList;
        };
        using StateDataPtr = std::shared_ptr<StateData>;
        
        StateDataPtr m_data;
    public:
        /// @brief Instantiates a Deferred with a Promise which is already resolved to v
        Deferred(T v) : m_data(std::make_shared<StateData>(v)) {}
        /// @brief Instantiates a Deferred with a Promise which is already rejected with e
        Deferred(std::exception_ptr ep) : m_data(std::make_shared<StateData>(ep)) {}
        /// @brief Instantiates a Deferred object with a pending Promise
        Deferred() : m_data(std::make_shared<StateData>()) {}
        /// @brief Creates an object with the shared data from the rh object (move)
        Deferred(Deferred<T> &&rh) : m_data(std::move(rh.m_data)) {} // Move constructor
        /// @brief Creates an object with the shared data from the rh object (copy)
        Deferred(const Deferred<T> &rh) : m_data(rh.m_data) {} // Copy constructor
        
        /// @brief Destroys the Deferred.  Note that this doesn't clean up Promise
        /// objects unless this is the last copy
        ~Deferred() {}
        
        /// @brief Copies another Deferred into this one, assuming its shared state
        Deferred<T> &operator=(const Deferred<T> &rh) {
            m_data = rh.m_data;
            return *this;
        }
        /// @brief Moves another Deferred into this one, assuming its shared state
        Deferred<T> &operator=(const Deferred<T> &&rh) {
            m_data = std::move(rh.m_data);
            return *this;
        }
        
        /// @brief Returns a FB::Promise<T> object controlled by this FB::Deferred
        /// object
        Promise<T> promise() const { return Promise<T>(m_data); }
        
        /// @brief invalidates this Deferred; if the object is still pending, reject
        /// it
        void invalidate() const {
            if (m_data->state == PromiseState::PENDING) {
                reject(std::make_exception_ptr(std::runtime_error("Deferred object destroyed: 2")));
            }
        }
        
        /// @brief Resolves all associated Promise objects to v
        void resolve(T v) const { m_data->resolve(v); }
        /// @brief All associated Promise objects with resolve or reject along with v
        void resolve(Promise<T> v) const {
            Deferred<T> dfd(*this);
            auto onDone = [dfd](T resV) { dfd.resolve(resV); };
            auto onFail = [dfd](std::exception_ptr e) { dfd.reject(e); };
            v.done(onDone, onFail);
        }
        /// @brief Rejects all associated Promise objects with e
        void reject(std::exception_ptr ep) const { m_data->reject(ep); }
    };
    
    /// @brief Template specialization of Deferred for type void; works slightly
    /// different from normal Deferred in that a Resolved state has no value
    template <> class Deferred<void> final 
    {
        friend class Promise<void>;
    public:
        using type = void;
        using Callback = std::function<void()>;
        using ErrCallback = std::function<void(std::exception_ptr ep)>;
        
    private:
        struct StateData {
            StateData(bool v) {
                state = v ? PromiseState::RESOLVED : PromiseState::PENDING;
            }
            StateData(std::exception_ptr ep) : state(PromiseState::REJECTED), err_ptr(ep) {}
            StateData() : state(PromiseState::PENDING) {}
            PromiseState state;
            std::exception_ptr err_ptr;
            
            std::vector<Callback> resolveList;
            std::vector<ErrCallback> rejectList;
        };
        using StateDataPtr = std::shared_ptr<StateData>;
        
        StateDataPtr m_data;
    public:
        /// @brief Instantiates a Deferred<void> with a Promise<void> which is
        /// resolved if v is true, else pending
        Deferred(bool v) : m_data(std::make_shared<StateData>(v)) {}
        /// @brief Instantiates a Deferred<void> with a Promise which is already
        /// rejected with e
        Deferred(std::exception_ptr ep) : m_data(std::make_shared<StateData>(ep)) {}
        /// @brief Instantiates a Deferred<void> objet with a pending Promise<void>
        Deferred() : m_data(std::make_shared<StateData>()) {}
        /// @brief Creates an object with the shared data from the rh object (move)
        Deferred(Deferred<void> &&rh) : m_data(std::move(rh.m_data)) {} // Move constructor
        /// @brief Creates an object with the shared data from the rh object (copy)
        Deferred(const Deferred<void> &rh) : m_data(rh.m_data) {} // Copy constructor
        
        /// @brief Destroys the Deferred<void>.  Note that this doesn't clean up
        /// Promise<void> objects unless this is the last copy
        ~Deferred() {}
        
        /// @brief Copies another Deferred<void> into this one, assuming its shared
        /// state
        Deferred<void> &operator=(const Deferred<void> &rh) {
            m_data = rh.m_data;
            return *this;
        }
        /// @brief Moves another Deferred<void> into this one, assuming its shared
        /// state
        Deferred<void> &operator=(const Deferred<void> &&rh) {
            m_data = std::move(rh.m_data);
            return *this;
        }
        
        /// @brief Returns a FB::Promise<void> object controlled by this FB::Deferred
        /// object
        Promise<void> promise() const;
        
        /// @brief invalidates this Deferred; if the object is still pending, reject
        /// it
        void invalidate() const;
        
        /// @brief All associated Promise<void> objects with resolve or reject along
        /// with v
        void resolve(Promise<void> v) const;
        /// @brief Resolves all associated Promise<void> objects with no value
        void resolve() const {
            m_data->state = PromiseState::RESOLVED;
            for (auto fn : m_data->resolveList) {
                fn();
            }
            m_data->resolveList.clear();
        }
        /// @brief Rejects all associated Promise objects with e
        void reject(std::exception_ptr ep) const {
            m_data->err_ptr = ep;
            m_data->state = PromiseState::REJECTED;
            for (auto fn : m_data->rejectList) {
                fn(ep);
            }
            m_data->rejectList.clear();
        }
    };
    
    template <typename Uout, typename Uin>
    inline typename std::enable_if<(!std::is_same<Uout, void>::value), FB::Promise<Uout> >::type
    _doPromiseThen(const Promise<Uin> &inP, std::function<Uout(Uin)> cbSuccess, std::function<Uout(std::exception_ptr)> cbFail);
    
    template <typename Uout, typename Uin>
    inline typename std::enable_if<std::is_same<Uout, void>::value, FB::Promise<void> >::type
    _doPromiseThen(const Promise<Uin> &inP, std::function<void(Uin)> cbSuccess, std::function<void(std::exception_ptr)> cbFail);
    
    template <typename T> 
    class Promise 
    {
    private:
        friend class Deferred<T>;
        typename Deferred<T>::StateDataPtr m_data;
        
        template <typename Uout>
        friend Promise<Uout> _doPromiseThen(const Promise<T> &inPromise, std::function<Uout(T)> cbSuccess, std::function<Uout(std::exception_ptr)> cbFail);
        
    public:
        /// @brief Creates an invalid Promise; useful only if you plan to use the
        /// assignment operator later
        Promise() {}
        Promise(const typename Deferred<T>::StateDataPtr data) : m_data(data) {}
        /// @brief Creates a Promise object using shared state from Promise rh (move)
        Promise(Promise &&rh) : m_data(std::move(rh.m_data)) {} // Move constructor
        /// @brief Creates a Promise object using shared state from Promise rh (copy)
        Promise(const Promise<T> &rh) : m_data(rh.m_data) {} // Copy constructor
        /// @brief The only valid way to create a Promise without a Deferred, creates
        /// a pre-resolved Promise
        Promise(T v) {
            Deferred<T> dfd{ v };
            m_data = dfd.promise().m_data;
        }
        
        /// @brief Assigns rh to this Promise, assuming all shared state from rh and
        /// discarding any current state
        ///
        /// Note that this will not invalidate any reject or resolve handlers unless this is the last Promise
        /// or Deferred object which exists with that shared state
        Promise<T> &operator=(const Promise<T> &rh) {
            m_data = rh.m_data;
            return *this;
        }
        Promise<T> &operator=(const Promise<T> &&rh) {
            m_data = std::move(rh.m_data);
            return *this;
        }
        
        /// @brief returns true if this is a valid Promise
        bool isValid() const { return static_cast<bool>(m_data); }
        
        // available only when creating a variantDeferred, allows forcing the type in the variant.
        // Unless you *really* know what you're doing, don't use this; it's used
        // internally. If you use this it will most likely cause convert_cast to not work.
        template <typename U, typename V = T, typename std::enable_if<std::is_same<FB::variant, V>::value, int>::type = 0>
        Promise(const Promise<U> &rh, bool force) {
            Deferred<T> dfd;
            auto onDone = [dfd](U v)->void {
                FB::variant val{ v, true };
                dfd.resolve(val);
            };
            auto onFail = [dfd](std::exception_ptr e) { dfd.reject(e); };
            rh.done(onDone, onFail);
            m_data = dfd.promise().m_data;
        }
        
        template <typename U, typename V = T, typename std::enable_if<std::is_same<FB::variant, V>::value, int>::type = 0>
        Promise(const Promise<U> &rh) {
            Deferred<T> dfd;
            auto onDone = [dfd](U v)->void {
                FB::variant val{ v };
                dfd.resolve(val);
            };
            auto onFail = [dfd](std::exception_ptr e) { dfd.reject(e); };
            rh.done(onDone, onFail);
            m_data = dfd.promise().m_data;
        }
        
        template <typename U, typename V = T, typename std::enable_if<!std::is_same<FB::variant, V>::value, int>::type = 0>
        Promise(const Promise<U> &rh) {
            Deferred<T> dfd;
            auto onDone = [dfd](U v)->void {
                FB::variant val{ v };
                dfd.resolve(val.convert_cast<T>());
            };
            auto onFail = [dfd](std::exception_ptr e) { dfd.reject(e); };
            rh.done(onDone, onFail);
            m_data = dfd.promise().m_data;
        }
        
        /// @brief Used to convert a Promise of one type to a Promise of another type
        ///
        /// This relies on FB::variant::convert_cast and can only be used to convert types
        /// which would succeed with a convert_cast. If an exception is thrown during conversion
        /// the returned Promise object will be rejected with that exception
        ///
        /// @return a Promise of the new type which will resolve after the original Promise resolves and a FB::variant::convert_cast succeeds
        template <typename U> 
        Promise<U> convert_cast() { 
            return Promise<U>(*this); 
        }
        
        /// @brief Returns a Promise object which is already rejected
        static Promise<T> rejected(std::exception_ptr ep) {
            Deferred<T> dfd;
            dfd.reject(ep);
            return dfd.promise();
        }
        
        /// @brief Invalidates the Promise object
        void invalidate() { 
            m_data.reset(); 
        }
        
        /// @brief Accepts a Success handler and a Fail handler, returns a new
        /// Promise<Uout> which resolves to the value returned from those handlers. (The handlers must each return this type)
        ///
        /// This is very useful when you know a command might fail but want to properly handle it
        /// or when you need to transform the value before using it. Either
        /// cbSuccess or cbFail can be nullptr if only one is desired. Often a lambda expression may
        /// be appropriate for one or both of these Callable types
        ///
        /// @param cbSuccess nullptr or any Callable target accepting one parameter of type T and returning a value of type Uout
        /// @param cbFail    nullptr or any Callable target accepting one parameter of type std::exception and returning a value of type Uout
        ///
        /// @see http://en.cppreference.com/w/cpp/utility/functional/function
        template <typename Uout>
        Promise<Uout> then(std::function<Uout(T)> cbSuccess, std::function<Uout(std::exception_ptr)> cbFail = nullptr) const {
            return _doPromiseThen<Uout, T>(*this, cbSuccess, cbFail);
        }
        
        /// @brief Accepts a Success handler and a Fail handler, returns a new
        /// Promise<Uout> which chains to a Promise<Uout> returned from those handlers. (The handlers must each return a Promise of this type)
        ///
        /// This is particularly useful when you need several asynchronous commands to execute one after another
        /// and then want to deal with the result in one place. Your handler will be Called with the result of
        /// this Promise and it can then return a Promise of a different type (Uout). The Promise returned
        /// by this function will resolve or reject when that Promise does. These can be chained together
        /// in some very powerful ways.  Often a lambad expression may be appropriate for one or both
        /// of these Callable types
        ///
        /// @param cbSuccess nullptr or any Callable target accepting one parameter of type T and returning a Promise of type Uout
        /// @param cbFail    nullptr or any Callable target accepting one parameter of type std::exception and returning a Promise of type Uout
        ///
        /// @see http://en.cppreference.com/w/cpp/utility/functional/function
        template <typename Uout>
        Promise<Uout> thenPipe(std::function<Promise<Uout>(T)> cbSuccess, std::function<Promise<Uout>(std::exception_ptr)> cbFail = nullptr) const {
            if (!m_data) {
                return Promise<Uout>::rejected(std::make_exception_ptr(std::runtime_error("Promise invalid")));
            }
            Deferred<Uout> dfd;
            auto onDone = [ dfd, cbSuccess ](T v)->void {
                try {
                    auto res = cbSuccess(v);
                    auto onDone2 = [dfd](Uout v) { dfd.resolve(v); };
                    auto onFail2 = [dfd](std::exception_ptr e) { dfd.reject(e); };
                    res.done(onDone2, onFail2);
                }
                catch (std::exception e) {
                    auto ep = std::current_exception();
                    dfd.reject(ep);
                }
            };
            
            if (cbFail) {
                auto onFail = [ dfd, cbFail ](std::exception_ptr e1)->void {
                    try {
                        auto res = cbFail(e1);
                        auto onDone2 = [dfd](Uout v) { dfd.resolve(v); };
                        auto onFail2 = [dfd](std::exception_ptr e) { dfd.reject(e); };
                        res.done(onDone2, onFail2);
                    }
                    catch (std::exception e2) {
                        auto ep2 = std::current_exception();
                        dfd.reject(ep2);
                    }
                };
                done(onDone, onFail);
            } else {
                auto onFail = [dfd](std::exception_ptr e1)->void {
                    dfd.reject(e1);
                };
                done(onDone, onFail);
            }
            return dfd.promise();
        }
        
        /// @brief registers a Callable handler to be called if/when the Promise resolves
        ///
        /// Optionally accepts a second parameter with a failure handler
        ///
        /// @param cbSuccess  nullptr or any Callable target accepting one parameters of type T and returning void
        /// @param cbFail     nullptr or any Callable target accepting one parameter of type std::exception and returning void
        const Promise<T> &done(typename Deferred<T>::Callback cbSuccess, typename Deferred<T>::ErrCallback cbFail = nullptr) const {
            if (!m_data) {
                throw std::runtime_error("Promise invalid");
            }
            if (cbFail) {
                fail(cbFail);
            }
            if (!cbSuccess) {
                return *this;
            }
            if (m_data->state == PromiseState::PENDING) {
                m_data->resolveList.emplace_back(cbSuccess);
            } else if (m_data->state == PromiseState::RESOLVED) {
                cbSuccess(m_data->value);
            }
            return *this;
        }
        /// @brief registers a Callable handler to be called if/when the Promise is rejected
        ///
        /// @param cbFail     nullptr or any Callable target accepting one parameter of type std::exception and returning void
        const Promise<T> &fail(typename Deferred<T>::ErrCallback cbFail) const {
            if (!m_data) {
                throw std::runtime_error("Promise invalid");
            }
            if (!cbFail) {
                return *this;
            }
            if (m_data->state == PromiseState::PENDING) {
                m_data->rejectList.emplace_back(cbFail);
            } else if (m_data->state == PromiseState::REJECTED) {
                cbFail(m_data->err_ptr);
            }
            return *this;
        }
        
    };
    
    /// @brief Template specialization of Promise for void type, resolve state has no value
    template <> // specialization for Promise<void>
    class Promise<void> 
    {
    private:
        friend class Deferred<void>;
        Deferred<void>::StateDataPtr m_data;
        
        template <typename Uout>
        friend Promise<Uout> _doPromiseThen(const Promise<void> &inPromise, std::function<Uout(void)> cbSuccess, std::function<Uout(std::exception_ptr)> cbFail);
        
    public:
        /// @brief Creates an invalid Promise<void>; useful only if you plan to use
        /// the assignment operator later
        Promise() {}
        Promise(Deferred<void>::StateDataPtr data) : m_data(data) {}
        /// @brief Creates a Promise<void? object using shared state from Promise rh
        /// (move)
        Promise(Promise &&rh) : m_data(std::move(rh.m_data)) {} // Move constructor
        /// @brief Creates a Promise<void> object using shared state from Promise rh
        /// (copy)
        Promise(const Promise &rh) : m_data(rh.m_data) {} // Copy constructor
        /// @brief The only valid way to create a Promise<void> without a
        /// Deferred<void>, creates a Promise<void> which is resolved if v is true,
        /// else pending
        Promise(bool v) {
            Deferred<void> dfd{ v };
            m_data = dfd.promise().m_data;
        }
        
        /// @brief Assigns rh to this Promise<void>, assuming all shared state from rh and discarding any current state
        ///
        /// Note that this will not invalidate any reject or resolve handlers unless this is the last Promise<void>
        /// or Deferred<void> object which exists with that shared state
        Promise<void> &operator=(const Promise<void> &rh) {
            m_data = rh.m_data;
            return *this;
        }
        
        /// @brief returns true if this is a valid Promise<void>
        bool isValid() const { return static_cast<bool>(m_data); }
        
        template <typename U> 
        Promise(const Promise<U> &rh) {
            Deferred<void> dfd;
            auto onDone = [dfd](U v) { dfd.resolve(); };
            auto onFail = [dfd](std::exception_ptr e) { dfd.reject(e); };
            done(onDone, onFail);
            m_data = dfd.promise().m_data;
        }
        
        /// @brief Used to convert a Promise<void> a Promise of another type
        ///
        /// Since Promise<void> does not have a value, this will result in a Promise which
        /// returns the default value of whatever type. It is included more for completeness than
        /// because we actually think it will be useful
        ///
        /// @return a Promise of the new type which will resolve after the original Promise<void> resolves
        template <typename U> 
        Promise<U> convert_cast() { 
            return Promise<U>(*this); 
        }
        
        /// @brief Returns a Promise<void> object which is already rejected
        static Promise<void> rejected(std::exception_ptr ep) {
            Deferred<void> dfd;
            dfd.reject(ep);
            return dfd.promise();
        }
        
        /// @brief Invalidates the Promise<void> object
        void invalidate() { m_data.reset(); }
        
        /// @brief Accepts a Success handler and a Fail handler, returns a new
        /// Promise<Uout> which resolves to the value returned from those handlers. (The handlers
        /// must each return this type)
        ///
        /// This is very useful when you know a command might fail but want to
        /// properly handle it or when you want to provide a value for when the Promise<void> resolves.
        /// Either cbSuccess or cbFail can be nullptr if only one is desired. Often a lambda
        /// expression may be appropriate for one or both of these Callable types
        ///
        /// @param cbSuccess nullptr or any Callable target accepting no parameters and returning a value of type Uout
        /// @param cbFail    nullptr or any Callable target accepting one parameter of type std::exception and returning a value of type Uout
        ///
        /// @see http://en.cppreference.com/w/cpp/utility/functional/function
        template <typename Uout>
        Promise<Uout> then(std::function<Uout()> cbSuccess, std::function<Uout(std::exception_ptr)> cbFail = nullptr) const {
            if (!m_data) {
                return Promise<Uout>::rejected(std::make_exception_ptr(std::runtime_error("Promise invalid")));
            }
            Deferred<Uout> dfd;
            auto onDone = [ dfd, cbSuccess ]()->void {
                try {
                    auto res = cbSuccess();
                    dfd.resolve(res);
                }
                catch (std::exception e) {
                    auto ep = std::current_exception();
                    dfd.reject(ep);
                }
            };
            if (cbFail) {
                auto onFail = [ dfd, cbFail ](std::exception_ptr e1)->void {
                    try {
                        auto res = cbFail(e1);
                        dfd.resolve(res);
                    }
                    catch (std::exception e2) {
                        auto ep2 = std::current_exception();
                        dfd.reject(ep2);
                    }
                };
                done(onDone, onFail);
            } else {
                auto onFail = [dfd](std::exception_ptr e1)->void {
                    dfd.reject(e1);
                };
                done(onDone, onFail);
            }
            return dfd.promise();
        }
        
        /// @brief Accepts a Success handler and a Fail handler, returns a new
        /// Promise<Uout> which chains to a Promise<Uout> returned from those handlers. (The
        /// handlers must each return a Promise of this type)
        ///
        /// This is particularly useful when you need several asynchronous commands to execute one after another
        /// and then want to deal with the result in one place. Your handler will be Called and
        /// it can then return a Promise of a different type (Uout). The Promise returned
        /// by this function will resolve or reject when that Promise does. These can be chained together
        /// in some very powerful ways.  Often a lambad expression may be appropriate for one or both of these Callable types
        ///
        /// @param cbSuccess nullptr or any Callable target accepting no parameters and returning a Promise of type Uout
        /// @param cbFail    nullptr or any Callable target accepting one parameter of type std::exception and returning a Promise of type Uout
        ///
        /// @see http://en.cppreference.com/w/cpp/utility/functional/function
        template <typename Uout>
        Promise<Uout> thenPipe(std::function<Promise<Uout>()> cbSuccess, std::function<Promise<Uout>(std::exception_ptr)> cbFail = nullptr) const {
            if (!m_data) {
                return Promise<Uout>::rejected(std::make_exception_ptr(std::runtime_error("Promise invalid")));
            }
            Deferred<Uout> dfd;
            auto onDone = [ dfd, cbSuccess ]()->void {
                try {
                    auto res = cbSuccess();
                    auto onDone2 = [dfd](Uout v) { dfd.resolve(v); };
                    auto onFail2 = [dfd](std::exception_ptr e) { dfd.reject(e); };
                    res.done(onDone2, onFail2);
                }
                catch (std::exception e) {
                    auto ep = std::current_exception();
                    dfd.reject(ep);
                }
            };
            
            if (cbFail) {
                auto onFail = [ dfd, cbFail ](std::exception_ptr e1)->void {
                    try {
                        auto res = cbFail(e1);
                        auto onDone2 = [dfd](Uout v) { dfd.resolve(v); };
                        auto onFail2 = [dfd](std::exception_ptr e) { dfd.reject(e); };
                        res.done(onDone2, onFail2);
                    }
                    catch (std::exception e2) {
                        auto ep2 = std::current_exception();
                        dfd.reject(ep2);
                    }
                };
                done(onDone, onFail);
            } else {
                auto onFail = [dfd](std::exception_ptr e1)->void {
                    dfd.reject(e1);
                };
                done(onDone, onFail);
            }
            return dfd.promise();
        }
        
        /// @brief registers a Callable handler to be called if/when the Promise<void> resolves
        ///
        /// Optionally accepts a second parameter with a failure handler
        ///
        /// @param cbSuccess  nullptr or any Callable target accepting no parameters and returning void
        /// @param cbFail     nullptr or any Callable target accepting one parameter of type std::exception and returning void
        const Promise<void> &done(Deferred<void>::Callback cbSuccess, Deferred<void>::ErrCallback cbFail = nullptr) const {
            if (!m_data) {
                throw std::runtime_error("Promise invalid");
            }
            if (cbFail) {
                fail(cbFail);
            }
            if (!cbSuccess) {
                return *this;
            }
            if (m_data->state == PromiseState::PENDING) {
                m_data->resolveList.emplace_back(cbSuccess);
            } else if (m_data->state == PromiseState::RESOLVED) {
                cbSuccess();
            }
            return *this;
        }
        /// @brief registers a Callable handler to be called if/when the Promise<void> is rejected
        ///
        /// @param cbFail     nullptr or any Callable target accepting one parameter of type std::exception and returning void
        const Promise<void> &fail(Deferred<void>::ErrCallback cbFail) const {
            if (!m_data) {
                throw std::runtime_error("Promise invalid");
            }
            if (!cbFail) {
                return *this;
            }
            if (m_data->state == PromiseState::PENDING) {
                m_data->rejectList.emplace_back(cbFail);
            } else if (m_data->state == PromiseState::REJECTED) {
                cbFail(m_data->err_ptr);
            }
            return *this;
        }
    };
    
    // non-void
    template <typename Uout, typename Uin>
    inline typename std::enable_if<(!std::is_same<Uout, void>::value), FB::Promise<Uout> >::type
    _doPromiseThen(const Promise<Uin> &inP, std::function<Uout(Uin)> cbSuccess, std::function<Uout(std::exception_ptr)> cbFail) {
        if (!inP.isValid()) {
            return Promise<Uout>::rejected(std::make_exception_ptr(std::runtime_error("Promise invalid")));
        }
        Deferred<Uout> dfd;
        auto onDone = [ dfd, cbSuccess ](Uin v)->void {
            try {
                auto res = cbSuccess(v);
                dfd.resolve(res);
            }
            catch (std::exception e) {
                auto ep = std::current_exception();
                dfd.reject(ep);
            }
        };
        if (cbFail) {
            auto onFail = [ dfd, cbFail ](std::exception_ptr e1)->void {
                try {
                    auto res = cbFail(e1);
                    dfd.resolve(res);
                }
                catch (std::exception e2) {
                    auto ep2 = std::current_exception();
                    dfd.reject(ep2);
                }
            };
            inP.done(onDone, onFail);
        } else {
            auto onFail = [dfd](std::exception_ptr e1)->void {
                dfd.reject(e1);
            };
            inP.done(onDone, onFail);
        }
        return dfd.promise();
    }
    
    // void
    template <typename Uout, typename Uin>
    inline typename std::enable_if<std::is_same<Uout, void>::value, FB::Promise<void> >::type
    _doPromiseThen(const Promise<Uin> &inP, std::function<void(Uin)> cbSuccess, std::function<void(std::exception_ptr)> cbFail) {
        if (!inP.isValid()) {
            return Promise<void>::rejected(std::make_exception_ptr(std::runtime_error("Promise invalid")));
        }
        Deferred<void> dfd(false);
        auto onDone = [ dfd, cbSuccess ](Uin v)->void {
            try {
                cbSuccess(v);
                dfd.resolve();
            }
            catch (std::exception e) {
                auto ep = std::current_exception();
                dfd.reject(ep);
            }
        };
        if (cbFail) {
            auto onFail = [ dfd, cbFail ](std::exception_ptr e1)->void {
                try {
                    cbFail(e1);
                    dfd.resolve();
                }
                catch (std::exception e2) {
                    auto ep2 = std::current_exception();
                    dfd.reject(ep2);
                }
            };
            inP.done(onDone, onFail);
        } else {
            auto onFail = [dfd](std::exception_ptr e1)->void {
                dfd.reject(e1);
            };
            inP.done(onDone, onFail);
        }
        return dfd.promise();
    }
}

#endif // H_FBDEFERRED
