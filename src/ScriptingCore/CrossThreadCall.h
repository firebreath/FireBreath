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

namespace FB {

    class SyncBrowserCall;
    class FunctorCall
    {
        virtual void call() = 0;
        friend class CrossThreadCall;
    };

    template<class Functor, class C>
    class FunctorCallImpl : public FunctorCall
    {
    public:
        FunctorCallImpl(C &cls, Functor &func) : reference(cls), func(func) { }
        void call() { retVal = func(); }
        typename Functor::result_type getResult() { return retVal; }

    protected:
        C reference;
        Functor func;
        typename Functor::result_type  retVal;

        friend class SyncBrowserCall;
    };

    class CrossThreadCall
    {
    public:
        template<class Functor>
        static typename Functor::result_type syncCall(FB::BrowserHostPtr &host, Functor func);

        template<class C, class Functor>
        static void asyncCall(FB::BrowserHostPtr &host, C obj, Functor func);

    protected:
        CrossThreadCall(FunctorCall* funct) : funct(funct), m_returned(false) { }

        static void asyncCallbackFunctor(void *userData);
        static void syncCallbackFunctor(void *userData);

        FunctorCall* funct;
        variant m_result;
        bool m_returned;

        boost::condition_variable m_cond;
        boost::mutex m_mutex;
    };

    template<class C, class Functor>
    static void CrossThreadCall::asyncCall(FB::BrowserHostPtr &host, C obj, Functor func)
    {
        FunctorCallImpl<Functor, C> *funct = new FunctorCallImpl<Functor, C>(obj, func);
        CrossThreadCall *call = new CrossThreadCall(funct);
        host->ScheduleAsyncCall(&CrossThreadCall::asyncCallbackFunctor, call);
    }

    template<class Functor>
    static typename Functor::result_type CrossThreadCall::syncCall(FB::BrowserHostPtr &host, Functor func)
    {
        typename Functor::result_type result;
        FB::variant varResult;

        FunctorCallImpl<Functor, bool> *funct = new FunctorCallImpl<Functor, bool>(false, func);
        if (!host->isMainThread())
        {
            CrossThreadCall *call = new CrossThreadCall(funct);
            {
                boost::unique_lock<boost::mutex> lock(call->m_mutex);
                host->ScheduleAsyncCall(&CrossThreadCall::syncCallbackFunctor, call);

                while (!call->m_returned) {
                    call->m_cond.wait(lock);
                }
                result = funct->getResult();
                varResult = funct->getResult();
            }
            delete call;
        } else {
            funct->call();
            result = funct->getResult();
        }
        delete funct;
        if (varResult.get_type() == typeid(FB::script_error)) {
            throw FB::script_error(varResult.cast<const FB::script_error>().what());
        }
        return result;
    }

};

#endif // H_FB_CROSSTHREADCALL