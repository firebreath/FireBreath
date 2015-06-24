/**********************************************************\
Original Author: Wojciech Walek

Created:    June 18, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_COMPROMISE
#define H_COMPROMISE

#include "APITypes.h"
#include "IDispatchAPI.h"
#include "ActiveXBrowserHost.h"
#include "Deferred.h"
#include <boost/noncopyable.hpp>

namespace FB { namespace ActiveX {
	FB_FORWARD_PTR(ComPromise);
	class ComPromise : public std::enable_shared_from_this<ComPromise>, boost::noncopyable
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
		IDispatchAPIPtr m_npDeferred;
		IDispatchAPIPtr m_npPromise;

    public:
		static ComPromisePtr create(ActiveXBrowserHostPtr host, FB::variantPromise promise);
		ComPromise(ActiveXBrowserHostPtr host, FB::variantPromise promise, PrivateOnly&&);
		virtual ~ComPromise(void);
		void init(ActiveXBrowserHostPtr host);

		IDispatchAPIPtr getComPromise() {
			return m_npPromise;
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