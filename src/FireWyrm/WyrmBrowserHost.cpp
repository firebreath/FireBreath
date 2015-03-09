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

using namespace FB::FireWyrm;

WyrmBrowserHost::WyrmBrowserHost(WyrmColony *module, const FW_INST spawnId)
    : module(module), m_spawnId(spawnId), m_nextObjId(1)
{

}

WyrmBrowserHost::~WyrmBrowserHost(void) {

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

Promise<FB::VariantList> WyrmBrowserHost::GetArrayValues(FB::JSObjectPtr obj) {
    throw new std::runtime_error("Not implemented");
}

Promise<FB::VariantMap> WyrmBrowserHost::GetObjectValues(FB::JSObjectPtr obj) {
    throw new std::runtime_error("Not implemented");
}

BrowserStreamPtr WyrmBrowserHost::_createStream(const BrowserStreamRequest& req) const {
    throw new std::runtime_error("Not implemented");
}

void FB::FireWyrm::WyrmBrowserHost::evaluateJavaScript(const std::string &script) {
    throw new std::runtime_error("Not implemented");
}
