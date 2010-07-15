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
#include "BrowserHostWrapper.h"
#include "BrowserObjectAPI.h"

using namespace FB;

JSAPI::JSAPI(void) : m_refCount(0), m_valid(true)
{
    registerEvent("onload");
}

JSAPI::~JSAPI(void)
{
}

void JSAPI::AddRef()
{
    ++m_refCount;
}

unsigned int JSAPI::Release()
{
    if (--m_refCount == 0) {
        delete this;
        return 0;
    } else {
        return m_refCount;
    }
}

void JSAPI::invalidate()
{
    m_valid = false;
}

void JSAPI::FireEvent(std::string eventName, std::vector<FB::variant>& args)
{
    if (!m_valid)   // When invalidated, do nothing more
        return;

    std::pair<EventMultiMap::iterator, EventMultiMap::iterator> range = m_eventMap.equal_range(eventName);

    for (EventMultiMap::iterator eventIt = range.first; eventIt != range.second; eventIt++) {
        eventIt->second->InvokeAsync("", args);
    }
    EventSingleMap::iterator fnd = m_defEventMap.find(eventName);
    if (fnd != m_defEventMap.end() && fnd->second.ptr() != NULL && fnd->second->getEventId() != NULL) {
        fnd->second->InvokeAsync("", args);
    }

    // Some events are registered as a jsapi object with a method of the same name as the event
    for (EventIFaceMap::iterator ifaceIt = m_evtIfaces.begin(); ifaceIt != m_evtIfaces.end(); ifaceIt++) {
        ifaceIt->second->InvokeAsync(eventName, args);
    }
}

bool JSAPI::HasEvent(std::string eventName)
{
    EventSingleMap::iterator fnd = m_defEventMap.find(eventName);
    if (fnd != m_defEventMap.end()) {
        return true;
    } else {
        return false;
    }
}

void JSAPI::registerEventMethod(std::string name, BrowserObjectAPI *event)
{
    std::pair<EventMultiMap::iterator, EventMultiMap::iterator> range = m_eventMap.equal_range(name);

    for (EventMultiMap::iterator it = range.first; it != range.second; it++) {
        if (it->second->getEventId() == event->getEventId()) {
            return; // Already registered
        }
    }
    m_eventMap.insert(EventPair(name, event));
}

void JSAPI::unregisterEventMethod(std::string name, BrowserObjectAPI *event)
{
    std::pair<EventMultiMap::iterator, EventMultiMap::iterator> range = m_eventMap.equal_range(name);

    for (EventMultiMap::iterator it = range.first; it != range.second; it++) {
        if (it->second->getEventId() == event->getEventId()) {
            m_eventMap.erase(it);
            return;
        }
    }
}

void JSAPI::registerEventInterface(BrowserObjectAPI *event)
{
    m_evtIfaces[static_cast<void*>(event)] = event;
}

void JSAPI::unregisterEventInterface(BrowserObjectAPI *event)
{
    EventIFaceMap::iterator fnd = m_evtIfaces.find(static_cast<void*>(event));
    m_evtIfaces.erase(fnd);
}

BrowserObjectAPI *JSAPI::getDefaultEventMethod(std::string name)
{
    EventSingleMap::iterator fnd = m_defEventMap.find(name);
    return fnd->second.ptr();
}

void JSAPI::setDefaultEventMethod(std::string name, BrowserObjectAPI *event)
{
    m_defEventMap[name] = event;
}

void JSAPI::registerEvent(const std::string &name)
{
    if(m_defEventMap.find(name) == m_defEventMap.end())
        m_defEventMap[name] = 0;
}
