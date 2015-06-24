/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 16, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/
  
#pragma once
#ifndef H_AXPROMISE
#define H_AXPROMISE

#include "APITypes.h"
#include "ActiveXBrowserHost.h"
#include "Deferred.h"
#include <boost/noncopyable.hpp>

namespace FB { namespace ActiveX {
    FB_FORWARD_PTR(AXPromise);
    class AXPromise : public std::enable_shared_from_this<AXPromise>, boost::noncopyable
    {
    private:
        struct PrivateOnly {};
        ActiveXBrowserHostPtr getHost() {
            ActiveXBrowserHostPtr ptr(m_browser.lock());
            if (!ptr) {
                Invalidate();
                throw std::bad_cast();
            }
            return ptr;
        }
        ActiveXBrowserHostWeakPtr m_browser;
        FB::variantPromise m_promise;
		IDispatchAPIPtr m_axDeferred;
		IDispatchAPIPtr m_axPromise;

    public:
        static AXPromisePtr create(ActiveXBrowserHostPtr host, FB::variantPromise promise);
        AXPromise(ActiveXBrowserHostPtr host, FB::variantPromise promise, PrivateOnly&&);
        virtual ~AXPromise(void);
        void init(ActiveXBrowserHostPtr host);

		IDispatchAPIPtr getAXPromise() {
            //getHost()->RetainObject(m_axPromise);
            return m_axPromise;
        }

        void Invalidate() {
            if (!m_settled) {
                _onError(std::runtime_error("Invalidated"));
            }
            m_promise.invalidate();
        }

        bool isSettled() {
            return m_settled;
        }

    private:
        bool m_settled;
        void onSuccess(FB::variant res);
        void onError(FB::variant res);
        void _onSuccess(FB::variant res);
        void _onError(FB::variant res);
    };
}; };

#endif