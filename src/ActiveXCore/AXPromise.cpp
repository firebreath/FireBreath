/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 16, 2009
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
#include "AXPromise.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH


using namespace FB::ActiveX;

// FireBreathPromise, from https://github.com/FireBreath/fbpromise
extern const char* fbPromiseJS;

AXPromise::AXPromise(ActiveXBrowserHostPtr host, FB::variantPromise promise, PrivateOnly&&) : m_browser(host), m_promise(promise), m_settled(false) {
}

AXPromisePtr AXPromise::create(ActiveXBrowserHostPtr host, FB::variantPromise promise) {
    AXPromisePtr ptr(std::make_shared<AXPromise>(host, promise, PrivateOnly()));

    ptr->init(host);
    return ptr;
}

void AXPromise::init(ActiveXBrowserHostPtr host) {
    host->assertMainThread();

/*    NPObject* window;
    NPVariant tmp;

    NPError nperr{ host->GetValue(NPNVWindowNPObject, (void**)&window) };
    if (nperr) {
        throw std::runtime_error("Could not connect to browser");
    }
*/
	m_axDeferred = host->getPromiseObject();
	
	/*
    NPIdentifier idPromise = host->GetStringIdentifier("promise");
    if (!host->GetProperty(m_axDeferred, idPromise, &tmp)) {
        throw std::runtime_error("Could not use deferred object");
    }
    m_axPromise = tmp.value.objectValue;
    host->RetainObject(m_axPromise);
    host->ReleaseVariantValue(&tmp);
    host->ReleaseObject(window);
	*/
	m_axPromise = host->getPromiseProperty(m_axDeferred, "promise");


    // Hook up to the promise
    auto onSuccess = std::bind(&AXPromise::onSuccess, shared_from_this(), std::placeholders::_1);
    auto onError = std::bind(&AXPromise::onError, shared_from_this(), std::placeholders::_1);
    m_promise.done(onSuccess);
    m_promise.fail(onError);
}

FB::ActiveX::AXPromise::~AXPromise(void) {
    try {
        getHost()->assertMainThread();
        Invalidate();
    } catch (...) {}
}

void AXPromise::onSuccess(FB::variant res) {
    auto _onSuccess = std::bind(&AXPromise::_onSuccess, this, res);
    getHost()->ScheduleOnMainThread(shared_from_this(), _onSuccess);
}

void AXPromise::onError(FB::variant res) {
    auto _onError = std::bind(&AXPromise::_onError, this, res);
    getHost()->ScheduleOnMainThread(shared_from_this(), _onError);
}

void AXPromise::_onSuccess(FB::variant res) {
    if (m_settled) return;
    ActiveXBrowserHostPtr host = getHost();
    host->assertMainThread();

    VARIANT value, retVal;
	host->getComVariant(&value, res);

    m_settled = true;
	host->InvokePromise(m_axDeferred, "resolve", FB::VariantList{ &value });
/*    NPIdentifier npId = host->GetStringIdentifier("resolve");
    host->Invoke(m_axDeferred, npId, &value, 1, &retVal);
    host->ReleaseVariantValue(&retVal);
    host->ReleaseObject(m_axDeferred);
*/
	m_axDeferred = nullptr;
}

void AXPromise::_onError(FB::variant res) {
    if (m_settled) return;
	ActiveXBrowserHostPtr host = getHost();
    host->assertMainThread();
	
    VARIANT reason, retVal;
	host->getComVariant(&reason, res);

    m_settled = true;
	host->InvokePromise(m_axDeferred, "reject", FB::VariantList{ &reason });
/*    NPIdentifier npId = host->GetStringIdentifier("reject");
    host->Invoke(m_axDeferred, npId, &reason, 1, &retVal);
    host->ReleaseVariantValue(&retVal);
    host->ReleaseObject(m_axDeferred);
	*/
    m_axDeferred = nullptr;
}
