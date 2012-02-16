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
#include "precompiled_headers.h" // On windows, everything above this line in PCH

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
        it != args.end(); ++it) {
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
        it != args.end(); ++it) {
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
    EventContextMap eventMap;
    EventIfaceContextMap evtIfaces;
    {
        boost::recursive_mutex::scoped_lock _l(m_eventMutex);
        eventMap = m_eventMap;
        evtIfaces = m_evtIfaces;
    }

    std::set<void*> contexts;
    {
        EventContextMap::iterator it(eventMap.begin());
        EventContextMap::iterator end(eventMap.end());
        for (; it != end; ++it) {
            bool first(true);
            std::pair<EventMultiMap::iterator, EventMultiMap::iterator> range = it->second.equal_range(eventName);
            for (EventMultiMap::const_iterator eventIt = range.first; eventIt != range.second; ++eventIt) {
                if (first && eventIt->second->isValid() && eventIt->second->supportsOptimizedCalls()) {
                    contexts.insert(it->first);
                    std::vector<FB::JSObjectPtr> handlers;
                    std::vector<FB::JSObjectPtr> ifaces;
                    for (EventMultiMap::const_iterator inIt(range.first); inIt != range.second; ++inIt) {
                        handlers.push_back(inIt->second);
                    }
                    EventIfaceContextMap::iterator ifCtx(evtIfaces.find(it->first));
                    if (ifCtx != evtIfaces.end()) {
                        for (EventIFaceMap::const_iterator ifaceIt(ifCtx->second.begin()); ifaceIt != ifCtx->second.end(); ++ifaceIt) {
                            ifaces.push_back(ifaceIt->second);
                        }
                    }
                    eventIt->second->callMultipleFunctions(eventName, args, handlers, ifaces);
                    break;
                } else {
                    if (eventIt->second->isValid()) {
                        first = false;
                        eventIt->second->InvokeAsync("", args);
                    }
                }
            }
        }
    }

    // Some events are registered as a jsapi object with a method of the same name as the event
    {
        EventIfaceContextMap::iterator it(evtIfaces.begin());
        EventIfaceContextMap::iterator end(evtIfaces.end());
        for (; it != end; ++it) {
            if (contexts.find(it->first) != contexts.end())
                continue; // We've already handled these

            for (EventIFaceMap::const_iterator ifaceIt = it->second.begin(); ifaceIt != it->second.end(); ++ifaceIt) {
                if (ifaceIt->second->isValid() && ifaceIt->second->supportsOptimizedCalls()) {
                    std::vector<FB::JSObjectPtr> handlers;
                    std::vector<FB::JSObjectPtr> ifaces;
                    for (EventIFaceMap::const_iterator inIt = it->second.begin(); inIt != it->second.end(); ++inIt) {
                        ifaces.push_back(inIt->second);
                    }
                    ifaceIt->second->callMultipleFunctions(eventName, args, handlers, ifaces);
                    break;
                }
                ifaceIt->second->InvokeAsync(eventName, args);
            }
        }
    }
}

void JSAPIImpl::FireEvent(const std::string& eventName, const std::vector<variant>& args)
{
    if (!m_valid)   // When invalidated, do nothing more
        return;

    {
        JSAPIImplPtr self(shared_from_this());
        boost::recursive_mutex::scoped_lock _l(m_proxyMutex);
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
            ++proxyIt;
        }
    }

    try {
        fireAsyncEvent(eventName, args);
    } catch (const FB::script_error&) {
        // a script_error can be fired during shutdown when this is called
        // from another thread; this should not be an error
    }
}

void JSAPIImpl::FireJSEvent( const std::string& eventName, const VariantMap &members, const VariantList &arguments )
{
    if (!m_valid)   // When invalidated, do nothing more
        return;

    {
        JSAPIImplPtr self(shared_from_this());
        boost::recursive_mutex::scoped_lock _l(m_proxyMutex);
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
            ++proxyIt;
        }
    }

    VariantList args;
    args.push_back(CreateEvent(shared_from_this(), eventName, members, arguments));

    {
        EventContextMap eventMap;
        {
            boost::recursive_mutex::scoped_lock _l(m_eventMutex);
            eventMap = m_eventMap;
        }

        EventContextMap::iterator it(eventMap.begin());
        while (it != m_eventMap.end()) {
            std::pair<EventMultiMap::iterator, EventMultiMap::iterator> range = it->second.equal_range(eventName);
            for (EventMultiMap::const_iterator eventIt = range.first; eventIt != range.second; ++eventIt) {
                eventIt->second->InvokeAsync("", args);
            }
            ++it;
        }
    }

    // Some events are registered as a jsapi object with a method of the same name as the event
    {
        EventIfaceContextMap evtIfaces;
        {
            boost::recursive_mutex::scoped_lock _l(m_eventMutex);
            evtIfaces = m_evtIfaces;
        }

        EventIfaceContextMap::iterator it(evtIfaces.begin());
        while (it != evtIfaces.end()) {
            for (EventIFaceMap::const_iterator ifaceIt = it->second.begin(); ifaceIt != it->second.end(); ++ifaceIt) {
                ifaceIt->second->InvokeAsync(eventName, args);
            }
        }
    }
}

void JSAPIImpl::registerEventMethod(const std::string& name, JSObjectPtr &event)
{
    if (!event)
        throw invalid_arguments();

    boost::recursive_mutex::scoped_lock _l(m_eventMutex);
    std::pair<EventMultiMap::iterator, EventMultiMap::iterator> range = m_eventMap[event->getEventContext()].equal_range(name);

    for (EventMultiMap::iterator it = range.first; it != range.second; ++it) {
        if (it->second->getEventId() == event->getEventId()) {
            return; // Already registered
        }
    }
    m_eventMap[event->getEventContext()].insert(EventPair(name, event));
}


void JSAPIImpl::unregisterEventMethod(const std::string& name, JSObjectPtr &event)
{
    if (!event)
        throw invalid_arguments();

    boost::recursive_mutex::scoped_lock _l(m_eventMutex);
    std::pair<EventMultiMap::iterator, EventMultiMap::iterator> range = m_eventMap[event->getEventContext()].equal_range(name);

    for (EventMultiMap::iterator it = range.first; it != range.second; ++it) {
        if (it->second->getEventId() == event->getEventId()) {
            m_eventMap[event->getEventContext()].erase(it);
            return;
        }
    }
}

void JSAPIImpl::registerProxy( const JSAPIImplWeakPtr &ptr ) const
{
    boost::recursive_mutex::scoped_lock _l(m_proxyMutex);
    m_proxies.push_back(ptr);
}

void JSAPIImpl::unregisterProxy( const JSAPIImplPtr& ptr ) const
{
    boost::recursive_mutex::scoped_lock _l(m_proxyMutex);
    ProxyList::iterator it = m_proxies.begin();
    while (it != m_proxies.end()) {
        JSAPIPtr cur(it->lock());
        if (!cur || ptr == cur)
            it = m_proxies.erase(it);
        else
            ++it;
    }
}
