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

#include <boost/bind.hpp>
#include "BrowserHost.h"
#include "JSObject.h"
#include "utf8_tools.h"
#include "JSEvent.h"

#include "JSAPI.h"

using namespace FB;

JSAPI::JSAPI(void) : m_valid(true)
{
    m_zoneStack.push_back(SecurityScope_Public);
    registerEvent("onload");
}

FB::JSAPI::JSAPI( const SecurityZone& securityLevel ) : m_valid(true)
{
    m_zoneStack.push_back(securityLevel);
    registerEvent("onload");
}

JSAPI::~JSAPI(void)
{
}

void JSAPI::invalidate()
{
    m_valid = false;
}

// Security zone management
void FB::JSAPI::setDefaultZone( const SecurityZone& securityLevel )
{
    assert(m_zoneStack.size() > 0);
    m_zoneStack.pop_front();
    m_zoneStack.push_front(securityLevel);
}
FB::SecurityZone FB::JSAPI::getDefaultZone() const
{
    assert(m_zoneStack.size() > 0);
    return m_zoneStack.front();
}

void FB::JSAPI::pushZone( const SecurityZone& securityLevel )
{
    m_zoneMutex.lock();
    m_zoneStack.push_back(securityLevel);
}

void FB::JSAPI::popZone( )
{
    m_zoneStack.pop_back();
    m_zoneMutex.unlock();
}

FB::SecurityZone FB::JSAPI::getZone() const
{
    assert(m_zoneStack.size() > 0);
    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);
    return m_zoneStack.back();
}


void JSAPI::FireEvent(const std::wstring& eventName, const std::vector<variant>& args)
{
    FireEvent(wstring_to_utf8(eventName), args);
}

FB::VariantMap proxyProcessMap( const FB::VariantMap &args, const FB::JSAPIPtr& self, const FB::JSAPIPtr& proxy );
FB::VariantList proxyProcessList( const FB::VariantList &args, FB::JSAPIPtr self, FB::JSAPIPtr proxy )
{
    FB::VariantList newArgs;
    for (FB::VariantList::const_iterator it = args.begin();
        it != args.end(); it++) {
        if (it->is_of_type<FB::JSAPIPtr>() && it->convert_cast<FB::JSAPIPtr>() == self) {
            newArgs.push_back(proxy);
        } else if (it->is_of_type<FB::VariantList>()) {
            newArgs.push_back(proxyProcessList(it->convert_cast<FB::VariantList>(), self, proxy));
        } else if (it->is_of_type<FB::VariantMap>()) {
            newArgs.push_back(proxyProcessMap(it->convert_cast<FB::VariantMap>(), self, proxy));
        } else {
            newArgs.push_back(*it);
        }
    }
    return newArgs;
}

FB::VariantMap proxyProcessMap( const FB::VariantMap &args, const FB::JSAPIPtr& self, const FB::JSAPIPtr& proxy )
{
    FB::VariantMap newMap;
    for (FB::VariantMap::const_iterator it = args.begin();
        it != args.end(); it++) {
        if (it->second.is_of_type<FB::JSAPIPtr>() && it->second.convert_cast<FB::JSAPIPtr>() == self) {
            newMap[it->first] = proxy;
        } else if (it->second.is_of_type<FB::VariantList>()) {
            newMap[it->first] = proxyProcessList(it->second.convert_cast<FB::VariantList>(), self, proxy);
        } else if (it->second.is_of_type<FB::VariantMap>()) {
            newMap[it->first] = proxyProcessMap(it->second.convert_cast<FB::VariantMap>(), self, proxy);
        } else {
            newMap[it->first] = it->second;
        }
    }
    return newMap;
}

void JSAPI::fireAsyncEvent( const std::string& eventName, const std::vector<variant>& args )
{
    std::pair<EventMultiMap::iterator, EventMultiMap::iterator> range = m_eventMap.equal_range(eventName);

    for (EventMultiMap::const_iterator eventIt = range.first; eventIt != range.second; eventIt++) {
        eventIt->second->InvokeAsync("", args);
    }
    EventSingleMap::const_iterator fnd = m_defEventMap.find(eventName);
    if (fnd != m_defEventMap.end() && fnd->second != NULL && fnd->second->getEventId() != NULL) {
        fnd->second->InvokeAsync("", args);
    }

    // Some events are registered as a jsapi object with a method of the same name as the event
    for (EventIFaceMap::const_iterator ifaceIt = m_evtIfaces.begin(); ifaceIt != m_evtIfaces.end(); ifaceIt++) {
        ifaceIt->second->InvokeAsync(eventName, args);
    }
}

void JSAPI::FireEvent(const std::string& eventName, const std::vector<variant>& args)
{
    if (!m_valid)   // When invalidated, do nothing more
        return;

    {
        FB::JSAPIPtr self(shared_ptr());
        ProxyList::iterator proxyIt = m_proxies.begin();
        while (proxyIt != m_proxies.end()) {
            FB::JSAPIPtr proxy(proxyIt->lock());
            if (!proxy) {
                // Since you can't use a shared_ptr in a destructor, there
                // is no way for the proxy object to let us know when it goes
                // away; thus when we find them, we remove them for efficiency
                proxyIt = m_proxies.erase(proxyIt);
                continue;
            }

            FB::VariantList newArgs = proxyProcessList(args, self, proxy);

            proxy->FireEvent(eventName, newArgs);
            proxyIt++;
        }
    }

    fireAsyncEvent(eventName, args);
}

void FB::JSAPI::FireJSEvent( const std::string& eventName, const FB::VariantMap &members, const FB::VariantList &arguments )
{
    if (!m_valid)   // When invalidated, do nothing more
        return;

    {
        FB::JSAPIPtr self(shared_ptr());
        ProxyList::iterator proxyIt = m_proxies.begin();
        while (proxyIt != m_proxies.end()) {
            FB::JSAPIPtr proxy(proxyIt->lock());
            if (!proxy) {
                // Since you can't use a shared_ptr in a destructor, there
                // is no way for the proxy object to let us know when it goes
                // away; thus when we find them, we remove them for efficiency
                proxyIt = m_proxies.erase(proxyIt);
                continue;
            }

            FB::VariantList newArgs = proxyProcessList(arguments, self, proxy);
            FB::VariantMap newMap = proxyProcessMap(members, self, proxy);

            proxy->FireJSEvent(eventName, newMap, newArgs);
            proxyIt++;
        }
    }
    
    FB::VariantList args;
    args.push_back(FB::CreateEvent(shared_ptr(), eventName, members, arguments));

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

void FB::JSAPI::FireJSEvent( const std::string& eventName, const FB::VariantMap &params )
{
    FireJSEvent(eventName, params, FB::VariantList());
}

void FB::JSAPI::FireJSEvent( const std::string& eventName, const FB::VariantList &arguments )
{
    FireJSEvent(eventName, FB::VariantMap(), arguments);
}


bool JSAPI::HasEvent(const std::wstring& eventName) const
{
    return HasEvent(wstring_to_utf8(eventName));
}

bool JSAPI::HasEvent(const std::string& eventName) const
{
    EventSingleMap::const_iterator fnd = m_defEventMap.find(eventName);
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

void JSAPI::registerEventInterface(const JSObjectPtr& event)
{
    m_evtIfaces[static_cast<void*>(event.get())] = event;
}

void JSAPI::unregisterEventInterface(const JSObjectPtr& event)
{
    EventIFaceMap::iterator fnd = m_evtIfaces.find(static_cast<void*>(event.get()));
    m_evtIfaces.erase(fnd);
}

JSObjectPtr JSAPI::getDefaultEventMethod(const std::wstring& name) const
{
    return getDefaultEventMethod(wstring_to_utf8(name));
}

JSObjectPtr JSAPI::getDefaultEventMethod(const std::string& name) const
{
    EventSingleMap::const_iterator fnd = m_defEventMap.find(name);
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

void JSAPI::getMemberNames(std::vector<std::wstring> &nameVector) const
{
    nameVector.clear();
    std::vector<std::string> utf8Vector;
    getMemberNames(utf8Vector);
    for (std::vector<std::string>::const_iterator it = utf8Vector.begin();
            it != utf8Vector.end(); ++it) {
        std::wstring wStrVal(utf8_to_wstring(*it));
        nameVector.push_back(wStrVal);
    }
}

bool JSAPI::HasMethod(const std::wstring& methodName) const
{
    return HasMethod(wstring_to_utf8(methodName));
}

bool FB::JSAPI::HasMethodObject( const std::wstring& methodObjName ) const
{
    return HasMethodObject(wstring_to_utf8(methodObjName));
}

bool JSAPI::HasProperty(const std::wstring& propertyName) const
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

void FB::JSAPI::registerProxy( const JSAPIWeakPtr &ptr ) const
{
    m_proxies.push_back(ptr);
}

void FB::JSAPI::unregisterProxy( const FB::JSAPIPtr& ptr ) const
{
    for (ProxyList::iterator it = m_proxies.begin(); it != m_proxies.end(); ++it) {
        FB::JSAPIPtr cur(it->lock());
        if (!cur || ptr == cur)
            it = m_proxies.erase(it);
    }
}

void FB::JSAPI::getMemberNames( std::vector<std::wstring> *nameVector ) const
{
    getMemberNames(*nameVector);
}

void FB::JSAPI::getMemberNames( std::vector<std::string> *nameVector ) const
{
    getMemberNames(*nameVector);
}

FB::JSAPIPtr FB::JSAPI::GetMethodObject( const std::wstring& methodObjName )
{
    return GetMethodObject(FB::wstring_to_utf8(methodObjName));
}
