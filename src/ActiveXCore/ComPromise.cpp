/**********************************************************\
Original Author: Wojciech Walek

Created:    June 18, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, FireBreath development team
\**********************************************************/

#include <typeinfo>
#include <algorithm>
#include <functional>
#include <memory>
#include "JSObject.h"

#include "precompiled_headers.h" // On windows, everything above this line in PCH
#include "ComPromise.h"

using namespace FB::ActiveX;

// FireBreathPromise, from https://github.com/FireBreath/fbpromise
extern const char* fbPromiseJS;

ComPromise::ComPromise(ActiveXBrowserHostPtr host, FB::variantPromise promise, PrivateOnly&&) : m_browser(host), m_promise(promise), m_settled(false) {
}

ComPromisePtr ComPromise::create(ActiveXBrowserHostPtr host, FB::variantPromise promise) {
	ComPromisePtr ptr(std::make_shared<ComPromise>(host, promise, PrivateOnly()));

    ptr->init(host);
    return ptr;
}

void ComPromise::init(ActiveXBrowserHostPtr host) {
    host->assertMainThread();

	m_npDeferred = host->getPromiseObject();

	try {
		m_npPromise = std::dynamic_pointer_cast<IDispatchAPI>(m_npDeferred->GetPropertySync("promise").cast<JSObjectPtr>());
	} catch (...) {
		throw std::runtime_error("Could not use deferred object");
	}

    // Hook up to the promise
	auto onSuccess = std::bind(&ComPromise::onSuccess, shared_from_this(), std::placeholders::_1);
	auto onError = std::bind(&ComPromise::onError, shared_from_this(), std::placeholders::_1);
    m_promise.done(onSuccess);
    m_promise.fail(onError);
}

FB::ActiveX::ComPromise::~ComPromise(void) {
    try {
        getHost()->assertMainThread();
        Invalidate();
    } catch (...) {}
}

void ComPromise::onSuccess(FB::variant res) {
	auto _onSuccess = std::bind(&ComPromise::_onSuccess, this, res);
    getHost()->ScheduleOnMainThread(shared_from_this(), _onSuccess);
}

void ComPromise::onError(FB::variant res) {
	auto _onError = std::bind(&ComPromise::_onError, this, res);
    getHost()->ScheduleOnMainThread(shared_from_this(), _onError);
}

void ComPromise::_onSuccess(FB::variant res) {
    if (m_settled) return;

	ActiveXBrowserHostPtr host = getHost();
    host->assertMainThread();

    m_settled = true;
	m_npDeferred->InvokeSync("resolve", FB::VariantList{ res });
	m_npDeferred = nullptr;
}

void ComPromise::_onError(FB::variant res) {
    if (m_settled) return;

	ActiveXBrowserHostPtr host = getHost();
    host->assertMainThread();

    m_settled = true;
	m_npDeferred->InvokeSync("reject", FB::VariantList{ res });
	m_npDeferred = nullptr;
}
