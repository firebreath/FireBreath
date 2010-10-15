/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Sept 24, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "JSAPI.h"
#include <boost/bind.hpp>
#include "BrowserHost.h"
#include "JSObject.h"
#include "utf8_tools.h"

using namespace FB;

JSAPI::JSAPI(void) : m_valid(true)
{
    registerEvent("onload");
}

JSAPI::~JSAPI(void)
{
}

void JSAPI::invalidate()
{
    m_valid = false;
}

void JSAPI::FireEvent(const std::wstring& eventName, const std::vector<variant>& args)
{
    FireEvent(wstring_to_utf8(eventName), args);
}

void JSAPI::FireEvent(const std::string& eventName, const std::vector<variant>& args)
{
    if (!m_valid)   // When invalidated, do nothing more
        return;

    std::pair<EventMultiMap::iterator, EventMultiMap::iterator> range = m_eventMap.equal_range(eventName);

    for (EventMultiMap::iterator eventIt = range.first; eventIt != range.second; eventIt++) {
        eventIt->second->InvokeAsync("", args);
    }
    EventSingleMap::iterator fnd = m_defEventMap.find(eventName);
    if (fnd != m_defEventMap.end() && fnd->second != NULL && fnd->second->getEventId() != NULL) {
        fnd->second->InvokeAsync("", args);
    }

    // Some events are registered as a jsapi object with a method of the same name as the event
    for (EventIFaceMap::iterator ifaceIt = m_evtIfaces.begin(); ifaceIt != m_evtIfaces.end(); ifaceIt++) {
        ifaceIt->second->InvokeAsync(eventName, args);
    }
}

bool JSAPI::HasEvent(const std::wstring& eventName)
{
    return HasEvent(wstring_to_utf8(eventName));
}

bool JSAPI::HasEvent(const std::string& eventName)
{
    EventSingleMap::iterator fnd = m_defEventMap.find(eventName);
    if (fnd != m_defEventMap.end()) {
        return true;
    } else {
        return false;
    }
}

void JSAPI::registerEventMethod(const std::wstring& name, JSObjectPtr &event)
{
    registerEventMethod(wstring_to_utf8(name), event);
}

void JSAPI::registerEventMethod(const std::string& name, JSObjectPtr &event)
{
    std::pair<EventMultiMap::iterator, EventMultiMap::iterator> range = m_eventMap.equal_range(name);

    for (EventMultiMap::iterator it = range.first; it != range.second; it++) {
        if (it->second->getEventId() == event->getEventId()) {
            return; // Already registered
        }
    }
    m_eventMap.insert(EventPair(name, event));
}

void JSAPI::unregisterEventMethod(const std::wstring& name, JSObjectPtr &event)
{
    unregisterEventMethod(wstring_to_utf8(name), event);
}

void JSAPI::unregisterEventMethod(const std::string& name, JSObjectPtr &event)
{
    std::pair<EventMultiMap::iterator, EventMultiMap::iterator> range = m_eventMap.equal_range(name);

    for (EventMultiMap::iterator it = range.first; it != range.second; it++) {
        if (it->second->getEventId() == event->getEventId()) {
            m_eventMap.erase(it);
            return;
        }
    }
}

void JSAPI::registerEventInterface(JSObjectPtr& event)
{
    m_evtIfaces[static_cast<void*>(event.get())] = event;
}

void JSAPI::unregisterEventInterface(JSObjectPtr& event)
{
    EventIFaceMap::iterator fnd = m_evtIfaces.find(static_cast<void*>(event.get()));
    m_evtIfaces.erase(fnd);
}

JSObjectPtr JSAPI::getDefaultEventMethod(const std::wstring& name)
{
    return getDefaultEventMethod(wstring_to_utf8(name));
}

JSObjectPtr JSAPI::getDefaultEventMethod(const std::string& name)
{
    EventSingleMap::iterator fnd = m_defEventMap.find(name);
    if (fnd != m_defEventMap.end()) {
        return fnd->second;
    }
    return JSObjectPtr();
}

void JSAPI::setDefaultEventMethod(const std::wstring& name, FB::JSObjectPtr event)
{
    setDefaultEventMethod(wstring_to_utf8(name), event);
}

void JSAPI::setDefaultEventMethod(const std::string& name, FB::JSObjectPtr event)
{
    if(event == NULL)
        m_defEventMap.erase(name);
    else 
        m_defEventMap[name] = event;
}

void JSAPI::registerEvent(const std::wstring &name)
{
    registerEvent(wstring_to_utf8(name));
}

void JSAPI::registerEvent(const std::string &name)
{
    if(m_defEventMap.find(name) == m_defEventMap.end())
        m_defEventMap[name] = JSObjectPtr();
}

void JSAPI::getMemberNames(std::vector<std::wstring> &nameVector)
{
    nameVector.clear();
    std::vector<std::string> utf8Vector;
    getMemberNames(utf8Vector);
    for (std::vector<std::string>::iterator it = utf8Vector.begin();
            it != utf8Vector.end(); ++it) {
        std::wstring wStrVal(utf8_to_wstring(*it));
        nameVector.push_back(wStrVal);
    }
}

bool JSAPI::HasMethod(const std::wstring& methodName)
{
    return HasMethod(wstring_to_utf8(methodName));
}

bool JSAPI::HasProperty(const std::wstring& propertyName)
{
    return HasProperty(wstring_to_utf8(propertyName));
}

variant JSAPI::GetProperty(const std::wstring& propertyName)
{
    return GetProperty(wstring_to_utf8(propertyName));
}

void JSAPI::SetProperty(const std::wstring& propertyName, const variant& value)
{
    SetProperty(wstring_to_utf8(propertyName), value);
}

variant JSAPI::Invoke(const std::wstring& methodName, const std::vector<variant>& args)
{
    return Invoke(wstring_to_utf8(methodName), args);
}
