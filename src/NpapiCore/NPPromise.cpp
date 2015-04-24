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

#include "precompiled_headers.h" // On windows, everything above this line in PCH
#include "NPPromise.h"

using namespace FB::Npapi;

// FireBreathPromise, from https://github.com/FireBreath/fbpromise
extern const char* fbPromiseJS;

NPPromise::NPPromise(NpapiBrowserHostPtr host, FB::variantPromise promise, PrivateOnly&&) : m_browser(host), m_promise(promise), m_settled(false) {
}

NPPromisePtr NPPromise::create(NpapiBrowserHostPtr host, FB::variantPromise promise) {
    NPPromisePtr ptr(std::make_shared<NPPromise>(host, promise, PrivateOnly()));

    ptr->init(host);
    return ptr;
}

void NPPromise::init(NpapiBrowserHostPtr host) {
    host->assertMainThread();

    NPObject* window;
    NPVariant tmp;

    NPError nperr{ host->GetValue(NPNVWindowNPObject, (void**)&window) };
    if (nperr) {
        throw std::runtime_error("Could not connect to browser");
    }

    m_npDeferred = host->getPromiseObject();

    NPIdentifier idPromise = host->GetStringIdentifier("promise");
    if (!host->GetProperty(m_npDeferred, idPromise, &tmp)) {
        throw std::runtime_error("Could not use deferred object");
    }
    m_npPromise = tmp.value.objectValue;
    host->RetainObject(m_npPromise);
    host->ReleaseVariantValue(&tmp);
    host->ReleaseObject(window);

    // Hook up to the promise
    auto onSuccess = std::bind(&NPPromise::onSuccess, shared_from_this(), std::placeholders::_1);
    auto onError = std::bind(&NPPromise::onError, shared_from_this(), std::placeholders::_1);
    m_promise.done(onSuccess);
    m_promise.fail(onError);
}

FB::Npapi::NPPromise::~NPPromise(void) {
    try {
        getHost()->assertMainThread();
        Invalidate();
    } catch (...) {}
}

void NPPromise::onSuccess(FB::variant res) {
    auto _onSuccess = std::bind(&NPPromise::_onSuccess, this, res);
    getHost()->ScheduleOnMainThread(shared_from_this(), _onSuccess);
}

void NPPromise::onError(FB::variant res) {
    auto _onError = std::bind(&NPPromise::_onError, this, res);
    getHost()->ScheduleOnMainThread(shared_from_this(), _onError);
}

void NPPromise::_onSuccess(FB::variant res) {
    if (m_settled) return;
    NpapiBrowserHostPtr host = getHost();
    host->assertMainThread();

    NPVariant value, retVal;
    host->getNPVariant(&value, res);

    m_settled = true;
    NPIdentifier npId = host->GetStringIdentifier("resolve");
    host->Invoke(m_npDeferred, npId, &value, 1, &retVal);
    host->ReleaseVariantValue(&retVal);
    host->ReleaseObject(m_npDeferred);
    m_npDeferred = nullptr;
}

void NPPromise::_onError(FB::variant res) {
    if (m_settled) return;
    NpapiBrowserHostPtr host = getHost();
    host->assertMainThread();

    NPVariant reason, retVal;
    host->getNPVariant(&reason, res);

    m_settled = true;
    NPIdentifier npId = host->GetStringIdentifier("reject");
    host->Invoke(m_npDeferred, npId, &reason, 1, &retVal);
    host->ReleaseVariantValue(&retVal);
    host->ReleaseObject(m_npDeferred);
    m_npDeferred = nullptr;
}
