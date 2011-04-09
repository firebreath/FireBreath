/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Sep 14, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_CROSSTHREADCALL
#define H_FB_CROSSTHREADCALL

#include <vector>
#include <string>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include "APITypes.h"
#include "JSObject.h"
#include "BrowserHost.h"
#include <boost/weak_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/scoped_ptr.hpp>
#include "logging.h"

namespace FB {

    class FunctorCall
    {
    public:
        virtual ~FunctorCall() {}
        virtual void call() = 0;
        friend class CrossThreadCall;
    };

    template<class Functor, class C, class RT = typename Functor::result_type>
    class FunctorCallImpl : public FunctorCall
    {
    public:
        FunctorCallImpl(const boost::shared_ptr<C> &cls, const Functor &func) : ref(true), reference(cls), func(func) { }
        FunctorCallImpl(const Functor &func) : ref(false), func(func) {}
        ~FunctorCallImpl() {
            FBLOG_TRACE("FunctorCall", "Destroying FunctorCall object (non-void)");
        }
        void call() {
            retVal = func();
        }
        RT getResult() { return retVal; }

    protected:
        bool ref;
        boost::shared_ptr<C> reference;
        Functor func;
        RT retVal;
    };

    template<class Functor, class C>
    class FunctorCallImpl<Functor, C, void> : public FunctorCall
    {
    public:
        FunctorCallImpl(const boost::shared_ptr<C> &cls, const Functor &func) : func(func), ref(true), reference(cls) { }
        FunctorCallImpl(const Functor &func) : func(func), ref(false) {}
        ~FunctorCallImpl() {
            FBLOG_TRACE("FunctorCall", "Destroying FunctorCall object (void)");
        }
        void call() {
            func();
        }

    protected:
        Functor func;
        bool ref;
        boost::shared_ptr<C> reference;
    };

    FB_FORWARD_PTR(CrossThreadCall);

    class CrossThreadCall
    {
    public:
        virtual ~CrossThreadCall() { }
        template<class Functor>
        static typename Functor::result_type syncCall(const FB::BrowserHostPtr &host, Functor func);

        template<class Functor>
        static typename Functor::result_type syncCallHelper(const FB::BrowserHostPtr &host, Functor func, boost::true_type /* is void */);
        template<class Functor>
        static typename Functor::result_type syncCallHelper(const FB::BrowserHostPtr &host, Functor func, boost::false_type /* is void */);

        template<class C, class Functor>
        static void asyncCall(const FB::BrowserHostPtr &host, const boost::shared_ptr<C>& obj, Functor func);

    protected:
        CrossThreadCall(const boost::shared_ptr<FunctorCall>& funct) : funct(funct), m_returned(false) { }

        static void asyncCallbackFunctor(void *userData);
        static void syncCallbackFunctor(void *userData);

        boost::shared_ptr<FunctorCall> funct;
        variant m_result;
        bool m_returned;

        boost::condition_variable m_cond;
        boost::mutex m_mutex;
    };

    template<class C, class Functor>
    void CrossThreadCall::asyncCall(const FB::BrowserHostPtr &host, const boost::shared_ptr<C>& obj, Functor func)
    {
        boost::shared_ptr<FunctorCall> funct = boost::make_shared<FunctorCallImpl<Functor, C> >(obj, func);
        CrossThreadCall *call = new CrossThreadCall(funct);
        if (!host->ScheduleAsyncCall(&CrossThreadCall::asyncCallbackFunctor, call)) {
            // Host is likely shut down; at any rate, this didn't work. Since it's asynchronous, fail silently
            delete call;
            return;
        }
    }

    template<class Functor>
    typename Functor::result_type CrossThreadCall::syncCall(const FB::BrowserHostPtr &host, Functor func)
    {
        typedef boost::is_same<void, typename Functor::result_type> is_void;
        return syncCallHelper(host, func, is_void());
    }

    template <class Functor>
    typename Functor::result_type CrossThreadCall::syncCallHelper(const FB::BrowserHostPtr &host, Functor func, boost::true_type /* return void */)
    {
        FB::variant varResult;

        // We make this shared so that if this is something that needs to be passed into the other thread,
        // it still goes away when everything is done with it
        boost::shared_ptr<FunctorCallImpl<Functor, bool> > funct = boost::make_shared<FunctorCallImpl<Functor, bool> >(func);
        if (!host->isMainThread())
        {
            // Synchronous call means that we want call to go away when this scope ends
            CrossThreadCallPtr call(new CrossThreadCall(funct));
            CrossThreadCallWeakPtr *callWeak = new CrossThreadCallWeakPtr(call);
            {
                boost::unique_lock<boost::mutex> lock(call->m_mutex);
                if (!host->ScheduleAsyncCall(&CrossThreadCall::syncCallbackFunctor, callWeak)) {
                    // Browser probably shutting down, but cross thread call failed.
                    delete callWeak;
                    throw FB::script_error("Could not marshal to main thread");
                }

                while (!call->m_returned && !host->isShutDown()) {
                    boost::posix_time::time_duration wait_duration = boost::posix_time::milliseconds(10);
                    call->m_cond.timed_wait(lock, wait_duration);
                }
                if (host->isShutDown())
                    throw FB::script_error("Shutting down");
                varResult = call->m_result;
            }
        } else {
            funct->call();
        }
        
        if (varResult.get_type() == typeid(FB::script_error*)) {
            FB::script_error* tmp(varResult.cast<FB::script_error*>());
            std::string msg = tmp->what();
            delete tmp;
            throw FB::script_error(varResult.cast<const FB::script_error>().what());
        }
    }

    template <class Functor>
    typename Functor::result_type CrossThreadCall::syncCallHelper(const FB::BrowserHostPtr &host, Functor func, boost::false_type /* return not void */)
    {
        typename Functor::result_type result;
        FB::variant varResult;

        // We make this shared so that if this is something that needs to be passed into the other thread,
        // it still goes away when everything is done with it
        boost::shared_ptr<FunctorCallImpl<Functor, bool> > funct = boost::make_shared<FunctorCallImpl<Functor, bool> >(func);
        if (!host->isMainThread())
        {
            // Synchronous call means that we want call to go away when this scope ends
            CrossThreadCallPtr call(new CrossThreadCall(funct));
            CrossThreadCallWeakPtr *callWeak = new CrossThreadCallWeakPtr(call);
            {
                boost::unique_lock<boost::mutex> lock(call->m_mutex);
                if (!host->ScheduleAsyncCall(&CrossThreadCall::syncCallbackFunctor, callWeak)) {
                    // Browser probably shutting down, but cross thread call failed.
                    delete callWeak;
                    throw FB::script_error("Could not marshal to main thread");
                }

                while (!call->m_returned && !host->isShutDown()) {
                    boost::posix_time::time_duration wait_duration = boost::posix_time::milliseconds(10);
                    call->m_cond.timed_wait(lock, wait_duration);
                }
                if (host->isShutDown())
                    throw FB::script_error("Shutting down");
                result = funct->getResult();
                varResult = call->m_result;
            }
        } else {
            funct->call();
            result = funct->getResult();
        }
        if (varResult.get_type() == typeid(FB::script_error*)) {
            FB::script_error* tmp(varResult.cast<FB::script_error*>());
            std::string msg = tmp->what();
            delete tmp;
            throw FB::script_error(msg);
        }
        return result;
    }

    
    template <class Functor>
    typename Functor::result_type BrowserHost::CallOnMainThread(Functor func)
    {
        boost::shared_lock<boost::shared_mutex> _l(m_xtmutex);
        return CrossThreadCall::syncCall(shared_from_this(), func);
    }
    
    template <class C, class Functor>
    void BrowserHost::ScheduleOnMainThread(const boost::shared_ptr<C>& obj, Functor func)
    {
        boost::shared_lock<boost::shared_mutex> _l(m_xtmutex);
        CrossThreadCall::asyncCall(shared_from_this(), obj, func);
    }    
};

#endif // H_FB_CROSSTHREADCALL

