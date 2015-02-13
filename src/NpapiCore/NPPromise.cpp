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
const char* fbPromiseJS = "(function(){var global=this;function isObject(obj){var type=typeof obj;return type==='function'||type==='object'&&!!obj}var toString=Object.prototype.toString;var isFunction=function(obj){return toString.call(obj)==='[object Function]'};if(typeof/./!=='function'){isFunction=function(obj){return typeof obj=='function'||false}}function defer(fn,arg){setTimeout(function(){fn(arg)},0)}var STATES={PEND:1,PROMISE:2,RESOLVE:3,REJECT:4};function DeferredObject(){var self=this;var fulfillHandlers=[];var rejectHandlers=[];var state=STATES.PEND;var value;self.promise={then:function(onFulfilled,onRejected){var promise2=new DeferredObject;function makeCallback(cbFunc,type){return function handleCallback(value){try{if(isFunction(cbFunc)){var x=cbFunc(value);promise2.resolve(x)}else{promise2[type](value)}}catch(e){promise2.reject(e)}}}if(state===STATES.RESOLVE){defer(function(){makeCallback(onFulfilled,'resolve')(value)})}else if(state===STATES.REJECT){defer(function(){makeCallback(onRejected,'reject')(value)})}else{fulfillHandlers.push(makeCallback(onFulfilled,'resolve'));rejectHandlers.push(makeCallback(onRejected,'reject'))}return promise2.promise}};function getThen(x){try{return isObject(x)?x.then:null}catch(e){self.reject(e);return false}}self.resolve=function(x,force){if(state===STATES.RESOLVE||state===STATES.REJECT){return}else if(x===self||x===self.promise){return self.reject(new TypeError)}else if(state!==STATES.PEND&&!force){return}var then=getThen(x);if(then===false){return}if(isFunction(then)){var scope_settled=false;var resolve=function(x){if(scope_settled){return}scope_settled=true;self.resolve(x,true)},reject=function(x){if(scope_settled){return}scope_settled=true;self.reject(x,true)};try{state=STATES.PROMISE;then.call(x,resolve,reject)}catch(e){if(!scope_settled){state=STATES.PEND;self.reject(e)}}}else{value=x;state=STATES.RESOLVE;for(var i=0;i<fulfillHandlers.length;++i){defer(fulfillHandlers[i],value)}}};self.reject=function(x){if(state===STATES.RESOLVE||state===STATES.REJECT){return}else if(x===self||x===self.promise){return self.reject(new TypeError)}value=x;state=STATES.REJECT;for(var i=0;i<rejectHandlers.length;++i){defer(rejectHandlers[i],value)}}}function makeDeferred(){return new DeferredObject}this.FireBreathPromise=makeDeferred;}).call(this);";

NPPromise::NPPromise(NpapiBrowserHostPtr host, FB::variantDeferredPtr promise, PrivateOnly&) : m_browser(host), m_promise(promise), m_settled(false) {
}

NPPromisePtr NPPromise::create(NpapiBrowserHostPtr host, FB::variantDeferredPtr promise) {
    NPPromisePtr ptr(std::make_shared<NPPromise>(host, promise, PrivateOnly()));

    ptr->init(host);
    return ptr;
}

void NPPromise::init(NpapiBrowserHostPtr host) {
    host->assertMainThread();

    NPObject* window;

    NPError nperr{ host->GetValue(NPNVWindowNPObject, (void**)&window) };
    if (nperr) {
        throw new std::runtime_error("Could not connect to browser");
    }
    NPVariant tmp;
    NPIdentifier idFbDeferred = host->GetStringIdentifier("FireBreathPromise");
    bool success = host->Invoke(window, idFbDeferred, nullptr, 0, &tmp);

    if (!success || tmp.type != NPVariantType_Object) {
        // We couldn't find the FireBreathPromise object already in the page
        NPVariant jsStr;
        host->getNPVariant(&jsStr, fbPromiseJS);
        if (!host->Evaluate(window, &jsStr.value.stringValue, &tmp)) {
            throw std::runtime_error("Could not inject Promise library");
        }
        host->ReleaseVariantValue(&tmp);
        success = host->Invoke(window, idFbDeferred, nullptr, 0, &tmp);
        if (!success || tmp.type != NPVariantType_Object) {
            throw std::runtime_error("Could not instantiate Promise");
        }
    }
    m_npDeferred = tmp.value.objectValue;
    host->RetainObject(m_npDeferred);
    host->ReleaseVariantValue(&tmp);

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
    m_promise->then(onSuccess, onError);
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
    auto _onError = std::bind(&NPPromise::_onSuccess, this, res);
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
