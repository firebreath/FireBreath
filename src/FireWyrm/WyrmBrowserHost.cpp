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

using namespace FB::FireWyrm;

WyrmBrowserHost::WyrmBrowserHost(WyrmColony *module, const FW_INST spawnId)
    : module(module), m_spawnId(spawnId), m_nextObjId(1)
{

}

WyrmBrowserHost::~WyrmBrowserHost(void) {

}

void WyrmBrowserHost::shutdown() {

}

bool WyrmBrowserHost::_scheduleAsyncCall(void(*func)(void *), void *userData) const {
    return module->_scheduleAsyncCall(func, userData);
}

FB::DOM::ElementPtr WyrmBrowserHost::getDOMElement() {
    throw new std::runtime_error("Not implemented");
}

FB::DOM::WindowPtr WyrmBrowserHost::getDOMWindow() {
    throw new std::runtime_error("Not implemented");
}

FB::DOM::DocumentPtr WyrmBrowserHost::getDOMDocument() {
    throw new std::runtime_error("Not implemented");
}

FB::Promise<FB::VariantList> WyrmBrowserHost::GetArrayValues(FB::JSObjectPtr obj) {
    throw new std::runtime_error("Not implemented");
}

FB::Promise<FB::VariantMap> WyrmBrowserHost::GetObjectValues(FB::JSObjectPtr obj) {
    throw new std::runtime_error("Not implemented");
}

FB::BrowserStreamPtr WyrmBrowserHost::_createStream(const FB::BrowserStreamRequest& req) const {
    throw new std::runtime_error("Not implemented");
}

void WyrmBrowserHost::evaluateJavaScript(const std::string &script) {
    throw new std::runtime_error("Not implemented");
}

int WyrmBrowserHost::delayedInvoke(const int delayms, const FB::JSObjectPtr& func, const FB::VariantList& args, std::string fname /*= ""*/) {
    throw new std::runtime_error("Not implemented");
}

LocalWyrmling WyrmBrowserHost::getWyrmling(FB::JSAPIPtr api) {
    FW_INST objId = m_nextObjId++;
    return createWyrmling(api, objId);
}

FB::FireWyrm::LocalWyrmling FB::FireWyrm::WyrmBrowserHost::getWyrmling(FB::JSAPIWeakPtr api) {
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

FB::VariantListPromise FB::FireWyrm::WyrmBrowserHost::Enum(FW_INST objId) {
    auto fnd = m_localMap.find(objId);
    if (fnd != m_localMap.end()) {
        return fnd->second.Enum();
    } else {
        throw std::runtime_error("Object not found");
    }
}

FB::variantPromise FB::FireWyrm::WyrmBrowserHost::Invoke(FW_INST objId, std::string name, FB::VariantList args) {
    auto fnd = m_localMap.find(objId);
    if (fnd != m_localMap.end()) {
        return fnd->second.Invoke(name, args);
    } else {
        throw std::runtime_error("Object not found");
    }
}

FB::variantPromise FB::FireWyrm::WyrmBrowserHost::GetP(FW_INST objId, std::string name) {
    auto fnd = m_localMap.find(objId);
    if (fnd != m_localMap.end()) {
        return fnd->second.GetP(name);
    } else {
        throw std::runtime_error("Object not found");
    }
}

FB::Promise<void> FB::FireWyrm::WyrmBrowserHost::SetP(FW_INST objId, std::string name, FB::variant value) {
    auto fnd = m_localMap.find(objId);
    if (fnd != m_localMap.end()) {
        return fnd->second.SetP(name, value);
    } else {
        throw std::runtime_error("Object not found");
    }
}

FB::Promise<void> FB::FireWyrm::WyrmBrowserHost::RelObj(FW_INST objId) {
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
    return module->DoCommand(cmd);
}
