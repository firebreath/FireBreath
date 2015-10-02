/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Mar 9, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2015 Richard Bateman, Firebreath development team
\**********************************************************/

#include "WyrmBrowserHost.h"
#include "BrowserStream.h"
#include "BrowserStreamRequest.h"
#include "LocalWyrmling.h"
#include "WyrmColony.h"
#include "Deferred.h"
#include "WyrmVariantUtil.h"
#include "DOM.h"

using namespace FB::FireWyrm;
using FB::Promise;
using FB::Deferred;

WyrmBrowserHost::WyrmBrowserHost(WyrmColony *module, const FW_INST spawnId)
    : module(module), m_spawnId(spawnId), m_nextObjId(1), m_browserObjId(0)
{

}

Promise<void> WyrmBrowserHost::init() {
    // Get the browser object
    auto self = std::dynamic_pointer_cast<WyrmBrowserHost>(shared_from_this());

    auto browserDfd = module->DoCommand(FB::VariantList{ "New", "browser", FB::VariantMap{} }, self);

    Deferred<void> dfdReady(false);
    browserDfd.done([self, dfdReady](FB::variant res) {
        // The return value should be the spawn id of the object
        FW_INST spawnId = res.convert_cast<FW_INST>();
        self->m_browserObjId = spawnId;
        auto larvae = std::make_shared<AlienLarvae>(self->module, spawnId, 0);
        auto Browser = self->m_Browser = AlienWyrmling::create(self, larvae);
        Browser->Invoke("getWindow", FB::VariantList{}).done([self,dfdReady](FB::variant var) {
            self->m_DOMWindow = var.convert_cast<FB::JSObjectPtr>();
            if (self->m_DOMDocument) {
                dfdReady.resolve();
            }
        });
        Browser->Invoke("getDocument", FB::VariantList{}).done([self,dfdReady](FB::variant var) {
            self->m_DOMDocument = var.convert_cast<FB::JSObjectPtr>();
            if (self->m_DOMWindow) {
                dfdReady.resolve();
            }
        });
    });

    m_onReady = dfdReady.promise();
    return m_onReady;
}

WyrmBrowserHost::~WyrmBrowserHost(void) {
}

void WyrmBrowserHost::shutdown() {
    m_DOMWindow.reset();
    m_DOMDocument.reset();
    module->DoCommand(FB::VariantList{ "Destroy", m_browserObjId });
}

bool WyrmBrowserHost::_scheduleAsyncCall(void(*func)(void *), void *userData) const {
    return module->_scheduleAsyncCall(func, userData);
}

FB::DOM::ElementPtr WyrmBrowserHost::getDOMElement() {
    throw std::runtime_error("Not implemented");
}

FB::DOM::WindowPtr WyrmBrowserHost::getDOMWindow() {
    return FB::DOM::Window::create(m_DOMWindow);
}

FB::DOM::DocumentPtr WyrmBrowserHost::getDOMDocument() {
    return FB::DOM::Document::create(m_DOMDocument);
}

FB::Promise<FB::VariantList> WyrmBrowserHost::GetArrayValues(FB::JSObjectPtr obj) {
    return m_onReady.thenPipe<FB::VariantList>([this, obj] () {
        return m_Browser->Invoke("readArray", FB::VariantList{obj}).then<FB::VariantList>([] (FB::variant ret) {
            return ret.cast<FB::VariantList>();
        });
    });
}

FB::Promise<FB::VariantMap> WyrmBrowserHost::GetObjectValues(FB::JSObjectPtr obj) {
    return m_onReady.thenPipe<FB::VariantMap>([this, obj] () {
        return m_Browser->Invoke("readObject", FB::VariantList{obj}).then<FB::VariantMap>([] (FB::variant ret) {
            return ret.cast<FB::VariantMap>();
        });
    });
}

FB::BrowserStreamPtr WyrmBrowserHost::_createStream(const FB::BrowserStreamRequest& req) const {
    throw std::runtime_error("Not implemented");
}

void WyrmBrowserHost::evaluateJavaScript(const std::string &script) {
    m_onReady.done([this, script] () {
        m_Browser->Invoke("evalFn", FB::VariantList{script});
    });
}

int WyrmBrowserHost::delayedInvoke(const int delayms, const FB::JSObjectPtr& func, const FB::VariantList& args, std::string fname /*= ""*/) {
    m_onReady.done([this, delayms, func, args, fname] () {
        m_Browser->Invoke("invokeWithDelay", FB::VariantList{delayms, func, args, fname});
    });
    return -1;
}

LocalWyrmling WyrmBrowserHost::getWyrmling(FB::JSAPIPtr api) {
    FW_INST objId = m_nextObjId++;
    return createWyrmling(api, objId);
}

LocalWyrmling WyrmBrowserHost::getWyrmling(FB::JSAPIWeakPtr api) {
    FW_INST objId = m_nextObjId++;
    return createWyrmling(api, objId);
}
LocalWyrmling WyrmBrowserHost::createWyrmling(FB::JSAPIPtr api, FW_INST objId) {
    LocalWyrmling obj( std::dynamic_pointer_cast<WyrmBrowserHost>(shared_from_this()), api, objId, true );
    m_localMap[objId] = obj;
    return obj;
}

LocalWyrmling WyrmBrowserHost::createWyrmling(FB::JSAPIWeakPtr api, FW_INST objId) {
    LocalWyrmling obj( std::dynamic_pointer_cast<WyrmBrowserHost>(shared_from_this()), api, objId, false );
    m_localMap[objId] = obj;
    return obj;
}

LocalWyrmling WyrmBrowserHost::getWyrmling(FW_INST objId) {
    auto fnd = m_localMap.find(objId);
    if (fnd != m_localMap.end()) {
        return fnd->second;
    } else {
        throw std::runtime_error("Object not found");
    }
}

FB::JSAPIPtr WyrmBrowserHost::getJSAPIFromWyrmling(FW_INST objId) {
    auto ling = getWyrmling(objId);
    return ling.getAPI();
}

bool WyrmBrowserHost::HasMethod(FW_INST objId, std::string name) {
    return getWyrmling(objId).HasMethod(name);
}

FB::VariantListPromise WyrmBrowserHost::Enum(FW_INST objId) {
    return getWyrmling(objId).Enum();
}

FB::variantPromise WyrmBrowserHost::Invoke(FW_INST objId, std::string name, FB::VariantList args) {
    return getWyrmling(objId).Invoke(name, args);
}

FB::variantPromise WyrmBrowserHost::GetP(FW_INST objId, std::string name) {
    return getWyrmling(objId).GetP(name);
}

FB::Promise<void> WyrmBrowserHost::SetP(FW_INST objId, std::string name, FB::variant value) {
    return getWyrmling(objId).SetP(name, value);
}

FB::Promise<void> WyrmBrowserHost::RelObj(FW_INST objId) {
    auto fnd = m_localMap.find(objId);
    if (fnd != m_localMap.end()) {
        fnd->second.Invalidate();
        m_localMap.erase(fnd);
        return FB::Promise<void>(true);
    } else {
        throw std::runtime_error("Object not found");
    }
}

FB::BrowserStreamPtr WyrmBrowserHost::_createUnsolicitedStream(const FB::BrowserStreamRequest& req) const {
    throw std::logic_error("Not implemented");
}

void WyrmBrowserHost::DoDeferredRelease() const {

}

bool WyrmBrowserHost::DetectProxySettings(std::map<std::string, std::string>& settingsMap, std::string url /*= ""*/) {
    throw std::logic_error("Not implemented");
}

FB::variantPromise WyrmBrowserHost::DoCommand(FB::VariantList cmd) {
    return module->DoCommand(cmd, std::dynamic_pointer_cast<WyrmBrowserHost>(shared_from_this()));
}
