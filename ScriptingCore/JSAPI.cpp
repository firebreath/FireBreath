/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Sept 24, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "JSAPI.h"
#include <boost/bind.hpp>
#include "BrowserHostWrapper.h"
#include "EventHandlerObject.h"

using namespace FB;

JSAPI::JSAPI(void) : m_valid(true), m_refCount(0)
{
    setDefaultEventMethod("onload", NULL);
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

void JSAPI::registerEventMethod(std::string name, EventHandlerObject *event)
{
    std::pair<EventMultiMap::iterator, EventMultiMap::iterator> range = m_eventMap.equal_range(name);

    for (EventMultiMap::iterator it = range.first; it != range.second; it++) {
        if (it->second->getEventId() == event->getEventId()) {
            return; // Already registered
        }
    }
    m_eventMap.insert(EventPair(name, event));
}

void JSAPI::unregisterEventMethod(std::string name, EventHandlerObject *event)
{
    std::pair<EventMultiMap::iterator, EventMultiMap::iterator> range = m_eventMap.equal_range(name);

    for (EventMultiMap::iterator it = range.first; it != range.second; it++) {
        if (it->second->getEventId() == event->getEventId()) {
            m_eventMap.erase(it);
            return;
        }
    }
}

void JSAPI::registerEventInterface(EventHandlerObject *event)
{
    m_evtIfaces[static_cast<void*>(event)] = event;
}

void JSAPI::unregisterEventInterface(EventHandlerObject *event)
{
    EventIFaceMap::iterator fnd = m_evtIfaces.find(static_cast<void*>(event));
    m_evtIfaces.erase(fnd);
}

EventHandlerObject *JSAPI::getDefaultEventMethod(std::string name)
{
    EventSingleMap::iterator fnd = m_defEventMap.find(name);
    return fnd->second.ptr();
}

void JSAPI::setDefaultEventMethod(std::string name, EventHandlerObject *event)
{
    m_defEventMap[name] = event;
}

void JSAPI::registerMethod(const std::string& name, CallMethodFunctor func)
{
	m_methodFunctorMap[name] = func;
}

void JSAPI::registerProperty(const std::string& name, GetPropFunctor getFunc, SetPropFunctor setFunc)
{
	m_propertyFunctorsMap[name] = PropertyFunctors(getFunc, setFunc);
}

bool JSAPI::HasMethod(std::string methodName)
{
	return (m_methodFunctorMap.find(methodName) != m_methodFunctorMap.end());
}

bool JSAPI::HasProperty(std::string propertyName)
{
	return (m_propertyFunctorsMap.find(propertyName) != m_propertyFunctorsMap.end());
}

bool JSAPI::HasProperty(int idx)
{
	// TODO: what is this method supposed to do?
	return false;
}

FB::variant JSAPI::GetProperty(std::string propertyName)
{
	PropertyFunctorsMap::const_iterator it = m_propertyFunctorsMap.find(propertyName);
	if(it == m_propertyFunctorsMap.end())
		return FB::variant();
	
	return it->second.get();
}

void JSAPI::SetProperty(std::string propertyName, const variant value)
{
	PropertyFunctorsMap::iterator it = m_propertyFunctorsMap.find(propertyName);
	if(it == m_propertyFunctorsMap.end())
		return;
	
	it->second.set(value);
}

FB::variant JSAPI::GetProperty(int idx)
{
	// TODO: what is this method supposed to do?
	return FB::variant();
}

void JSAPI::SetProperty(int idx, const variant value)
{
	// TODO: what is this method supposed to do?
}

FB::variant JSAPI::Invoke(std::string methodName, std::vector<variant> &args)
{
	MethodFunctorMap::iterator it = m_methodFunctorMap.find(methodName);
	if(it == m_methodFunctorMap.end())
		return FB::variant();

	return it->second(args);
}