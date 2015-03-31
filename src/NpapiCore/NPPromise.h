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
#ifndef H_NPPROMISE
#define H_NPPROMISE

#include "APITypes.h"
#include "NpapiTypes.h"
#include "NpapiBrowserHost.h"
#include "Deferred.h"
#include <boost/noncopyable.hpp>

namespace FB { namespace Npapi {
    FB_FORWARD_PTR(NPPromise);
    class NPPromise : public std::enable_shared_from_this<NPPromise>, boost::noncopyable
    {
    private:
        struct PrivateOnly {};
        NpapiBrowserHostPtr getHost() {
            NpapiBrowserHostPtr ptr(m_browser.lock());
            if (!ptr) {
                Invalidate();
                throw std::bad_cast();
            }
            return ptr;
        }
        NpapiBrowserHostWeakPtr m_browser;
        FB::variantPromise m_promise;
        NPObject* m_npDeferred;
        NPObject* m_npPromise;

    public:
        static NPPromisePtr create(NpapiBrowserHostPtr host, FB::variantPromise promise);
        NPPromise(NpapiBrowserHostPtr host, FB::variantPromise promise, PrivateOnly&&);
        virtual ~NPPromise(void);
        void init(NpapiBrowserHostPtr host);

        NPObject* getNPPromise() {
            getHost()->RetainObject(m_npPromise);
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