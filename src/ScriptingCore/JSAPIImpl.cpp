/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    April 8, 2011
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

#include "JSAPIImpl.h"

using namespace FB;

JSAPIImpl::JSAPIImpl(void) : m_valid(true)
{
    m_zoneStack.push_back(SecurityScope_Public);
    registerEvent("onload");
}

JSAPIImpl::JSAPIImpl( const SecurityZone& securityLevel ) : m_valid(true)
{
    m_zoneStack.push_back(securityLevel);
    registerEvent("onload");
}

JSAPIImpl::~JSAPIImpl(void)
{
}

void JSAPIImpl::invalidate()
{
    m_valid = false;
}

VariantMap proxyProcessMap( const VariantMap &args, const JSAPIImplPtr& self, const JSAPIImplPtr& proxy );
VariantList proxyProcessList( const VariantList &args, const JSAPIImplPtr& self, const JSAPIImplPtr& proxy )
{
    VariantList newArgs;
    for (VariantList::const_iterator it = args.begin();
        it != args.end(); it++) {
        if (it->is_of_type<JSAPIPtr>() && it->convert_cast<JSAPIPtr>() == self) {
            newArgs.push_back(proxy);
        } else if (it->is_of_type<VariantList>()) {
            newArgs.push_back(proxyProcessList(it->convert_cast<VariantList>(), self, proxy));
        } else if (it->is_of_type<VariantMap>()) {
            newArgs.push_back(proxyProcessMap(it->convert_cast<VariantMap>(), self, proxy));
        } else {
            newArgs.push_back(*it);
        }
    }
    return newArgs;
}

VariantMap proxyProcessMap( const VariantMap &args, const JSAPIImplPtr& self, const JSAPIImplPtr& proxy )
{
    VariantMap newMap;
    for (VariantMap::const_iterator it = args.begin();
        it != args.end(); it++) {
        if (it->second.is_of_type<JSAPIPtr>() && it->second.convert_cast<JSAPIPtr>() == self) {
            newMap[it->first] = proxy;
        } else if (it->second.is_of_type<VariantList>()) {
            newMap[it->first] = proxyProcessList(it->second.convert_cast<VariantList>(), self, proxy);
        } else if (it->second.is_of_type<VariantMap>()) {
            newMap[it->first] = proxyProcessMap(it->second.convert_cast<VariantMap>(), self, proxy);
        } else {
            newMap[it->first] = it->second;
        }
    }
    return newMap;
}

void JSAPIImpl::fireAsyncEvent( const std::string& eventName, const std::vector<variant>& args )
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

void JSAPIImpl::FireEvent(const std::string& eventName, const std::vector<variant>& args)
{
    if (!m_valid)   // When invalidated, do nothing more
        return;

    {
        JSAPIImplPtr self(shared_from_this());
        ProxyList::iterator proxyIt = m_proxies.begin();
        while (proxyIt != m_proxies.end()) {
            JSAPIImplPtr proxy(proxyIt->lock());
            if (!proxy) {
                // Since you can't use a shared_ptr in a destructor, there
                // is no way for the proxy object to let us know when it goes
                // away; thus when we find them, we remove them for efficiency
                proxyIt = m_proxies.erase(proxyIt);
                continue;
            }

            VariantList newArgs = proxyProcessList(args, self, proxy);

            proxy->FireEvent(eventName, newArgs);
            proxyIt++;
        }
    }

    fireAsyncEvent(eventName, args);
}

void JSAPIImpl::FireJSEvent( const std::string& eventName, const VariantMap &members, const VariantList &arguments )
{
    if (!m_valid)   // When invalidated, do nothing more
        return;

    {
        JSAPIImplPtr self(shared_from_this());
        ProxyList::iterator proxyIt = m_proxies.begin();
        while (proxyIt != m_proxies.end()) {
            JSAPIImplPtr proxy(proxyIt->lock());
            if (!proxy) {
                // Since you can't use a shared_ptr in a destructor, there
                // is no way for the proxy object to let us know when it goes
                // away; thus when we find them, we remove them for efficiency
                proxyIt = m_proxies.erase(proxyIt);
                continue;
            }

            VariantList newArgs = proxyProcessList(arguments, self, proxy);
            VariantMap newMap = proxyProcessMap(members, self, proxy);

            proxy->FireJSEvent(eventName, newMap, newArgs);
            proxyIt++;
        }
    }
    
    VariantList args;
    args.push_back(CreateEvent(shared_from_this(), eventName, members, arguments));

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

bool JSAPIImpl::HasEvent(const std::string& eventName) const
{
    EventSingleMap::const_iterator fnd = m_defEventMap.find(eventName);
    if (fnd != m_defEventMap.end()) {
        return true;
    } else {
        return false;
    }
}

void JSAPIImpl::registerEventMethod(const std::string& name, JSObjectPtr &event)
{
    if (!event)
        throw invalid_arguments();

    std::pair<EventMultiMap::iterator, EventMultiMap::iterator> range = m_eventMap.equal_range(name);

    for (EventMultiMap::iterator it = range.first; it != range.second; it++) {
        if (it->second->getEventId() == event->getEventId()) {
            return; // Already registered
        }
    }
    m_eventMap.insert(EventPair(name, event));
}


void JSAPIImpl::unregisterEventMethod(const std::string& name, JSObjectPtr &event)
{
    if (!event)
        throw invalid_arguments();

    std::pair<EventMultiMap::iterator, EventMultiMap::iterator> range = m_eventMap.equal_range(name);

    for (EventMultiMap::iterator it = range.first; it != range.second; it++) {
        if (it->second->getEventId() == event->getEventId()) {
            m_eventMap.erase(it);
            return;
        }
    }
}

JSObjectPtr JSAPIImpl::getDefaultEventMethod(const std::string& name) const
{
    EventSingleMap::const_iterator fnd = m_defEventMap.find(name);
    if (fnd != m_defEventMap.end()) {
        return fnd->second;
    }
    return JSObjectPtr();
}


void JSAPIImpl::setDefaultEventMethod(const std::string& name, JSObjectPtr event)
{
    if(event == NULL)
        m_defEventMap.erase(name);
    else 
        m_defEventMap[name] = event;
}

void JSAPIImpl::registerEvent(const std::string &name)
{
    if(m_defEventMap.find(name) == m_defEventMap.end())
        m_defEventMap[name] = JSObjectPtr();
}

void JSAPIImpl::registerProxy( const JSAPIImplWeakPtr &ptr ) const
{
    m_proxies.push_back(ptr);
}

void JSAPIImpl::unregisterProxy( const JSAPIImplPtr& ptr ) const
{
    for (ProxyList::iterator it = m_proxies.begin(); it != m_proxies.end(); ++it) {
        JSAPIPtr cur(it->lock());
        if (!cur || ptr == cur)
            it = m_proxies.erase(it);
    }
}
