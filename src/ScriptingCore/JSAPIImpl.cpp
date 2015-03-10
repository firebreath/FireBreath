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

#include <boost/range/iterator_range.hpp>
#include "BrowserHost.h"
#include "JSObject.h"
#include "utf8_tools.h"
#include "JSEvent.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include "JSAPIImpl.h"

using namespace FB;

JSAPIImpl::JSAPIImpl(void) : m_valid(true) {
    m_zoneStack.emplace_back(SecurityScope_Public);
    registerEvent("onload");
}

JSAPIImpl::JSAPIImpl(const SecurityZone& securityLevel) : m_valid(true) {
    m_zoneStack.emplace_back(securityLevel);
    registerEvent("onload");
}

JSAPIImpl::~JSAPIImpl(void) {
}

void JSAPIImpl::invalidate() {
    m_valid = false;
}

VariantMap proxyProcessMap(const VariantMap &args, const JSAPIImplPtr& self, const JSAPIImplPtr& proxy);
VariantList proxyProcessList(const VariantList &args, const JSAPIImplPtr& self, const JSAPIImplPtr& proxy) {
    VariantList newArgs;
    for (auto arg : args) {
        if (arg.is_of_type<JSAPIPtr>() && arg.cast<JSAPIPtr>() == self) {
            newArgs.emplace_back(proxy);
        } else if (arg.is_of_type<VariantList>()) {
            newArgs.emplace_back(proxyProcessList(arg.cast<VariantList>(), self, proxy));
        } else if (arg.is_of_type<VariantMap>()) {
            newArgs.emplace_back(proxyProcessMap(arg.cast<VariantMap>(), self, proxy));
        } else {
            newArgs.emplace_back(arg);
        }
    }
    return newArgs;
}

VariantMap proxyProcessMap(const VariantMap &args, const JSAPIImplPtr& self, const JSAPIImplPtr& proxy) {
    VariantMap newMap;
    for (auto arg : args) {
        if (arg.second.is_of_type<JSAPIPtr>() && arg.second.cast<JSAPIPtr>() == self) {
            newMap[arg.first] = proxy;
        } else if (arg.second.is_of_type<VariantList>()) {
            newMap[arg.first] = proxyProcessList(arg.second.cast<VariantList>(), self, proxy);
        } else if (arg.second.is_of_type<VariantMap>()) {
            newMap[arg.first] = proxyProcessMap(arg.second.cast<VariantMap>(), self, proxy);
        } else {
            newMap[arg.first] = arg.second;
        }
    }
    return newMap;
}

void JSAPIImpl::fireAsyncEvent(std::string eventName, const std::vector<variant>& args) {
    using JSObjectList = std::vector < FB::JSObjectPtr > ;
    EventContextMap eventMap;
    EventIfaceContextMap evtIfaces;
    {
        std::unique_lock<std::recursive_mutex> _l(m_eventMutex);
        eventMap = m_eventMap;
        evtIfaces = m_evtIfaces;
    }

    std::set<void*> contexts;
    for (auto ctx : eventMap) {
        bool first(true);
        void* ctxId(ctx.first);
        // ctx.second is the multimap of event name -> object
        // Search inside ctx.second for all matching event handler objects
        auto handlerList = boost::make_iterator_range(ctx.second.equal_range(eventName));
        for (auto handler : handlerList) {
            if (first && handler.second->isValid() && handler.second->supportsOptimizedCalls()) {
                contexts.insert(ctxId);
                JSObjectList handlers;
                JSObjectList ifaces;
                for (auto inItem : handlerList) {
                    handlers.emplace_back(inItem.second);
                }
                auto ifCtx(evtIfaces.find(ctxId));
                if (ifCtx != evtIfaces.end()) {
                    for (auto ifaceCur : ifCtx->second) {
                        ifaces.emplace_back(ifaceCur.second);
                    }
                }
                handler.second->callMultipleFunctions(eventName, args, handlers, ifaces);
                break;
            } else {
                if (handler.second->isValid()) {
                    first = false;
                    handler.second->Invoke("", args);
                }
            }
        }
    }

    // Some events are registered as a jsapi object with a method of the same name as the event
    for (auto ifaceGrp : evtIfaces) {
        if (contexts.find(ifaceGrp.first) != contexts.end())
            continue; // We've already handled these

        for (auto iface : ifaceGrp.second) {
            if (iface.second->isValid() && iface.second->supportsOptimizedCalls()) {
                std::vector<FB::JSObjectPtr> handlers;
                std::vector<FB::JSObjectPtr> ifaces;
                for (auto ifaceCur : ifaceGrp.second) {
                    ifaces.emplace_back(ifaceCur.second);
                }
                iface.second->callMultipleFunctions(eventName, args, handlers, ifaces);
                break;
            }
            iface.second->Invoke(eventName, args);
        }
    }
}

void JSAPIImpl::FireEvent(std::string eventName, const std::vector<variant>& args) {
    if (!m_valid)   // When invalidated, do nothing more
        return;

    {
        JSAPIImplPtr self(shared_from_this());
        std::unique_lock<std::recursive_mutex> _l(m_proxyMutex);
        ProxyList::iterator proxyIt = m_proxies.begin();
        for (auto proxyWeakPtr : m_proxies) {
            JSAPIImplPtr proxy(proxyWeakPtr.lock());
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

void JSAPIImpl::FireJSEvent(std::string eventName, const VariantMap &members, const VariantList &arguments) {
    if (!m_valid)   // When invalidated, do nothing more
        return;

    {
        JSAPIImplPtr self(shared_from_this());
        std::unique_lock<std::recursive_mutex> _l(m_proxyMutex);
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
    args.emplace_back(CreateEvent(shared_from_this(), eventName, members, arguments));

    {
        EventContextMap eventMap;
        {
            std::unique_lock<std::recursive_mutex> _l(m_eventMutex);
            eventMap = m_eventMap;
        }

        EventContextMap::iterator it(eventMap.begin());
        while (it != eventMap.end()) {
            auto range = boost::make_iterator_range(it->second.equal_range(eventName));
            for (auto item : range) {
                item.second->Invoke("", args);
            }
            ++it;
        }
    }

    // Some events are registered as a jsapi object with a method of the same name as the event
    {
        EventIfaceContextMap evtIfaces;
        {
            std::unique_lock<std::recursive_mutex> _l(m_eventMutex);
            evtIfaces = m_evtIfaces;
        }

        EventIfaceContextMap::iterator it(evtIfaces.begin());
        while (it != evtIfaces.end()) {
            for (auto ifaceIt = it->second.begin(); ifaceIt != it->second.end(); ++ifaceIt) {
                ifaceIt->second->Invoke(eventName, args);
            }
        }
    }
}

void JSAPIImpl::registerEventMethod(std::string name, JSObjectPtr &event) {
    if (!event)
        throw invalid_arguments();

    std::unique_lock<std::recursive_mutex> _l(m_eventMutex);
    auto range = boost::make_iterator_range(m_eventMap[event->getEventContext()].equal_range(name));

    for (auto grp : range) {
        if (grp.second->getEventId() == event->getEventId()) {
            return; // Already registered
        }
    }
    m_eventMap[event->getEventContext()].insert(EventPair(name, event));
}


void JSAPIImpl::unregisterEventMethod(std::string name, JSObjectPtr &event) {
    if (!event)
        throw invalid_arguments();

    std::unique_lock<std::recursive_mutex> _l(m_eventMutex);
    std::pair<EventMultiMap::iterator, EventMultiMap::iterator> range = m_eventMap[event->getEventContext()].equal_range(name);

    for (EventMultiMap::iterator it = range.first; it != range.second; ++it) {
        if (it->second->getEventId() == event->getEventId()) {
            m_eventMap[event->getEventContext()].erase(it);
            return;
        }
    }
}

void JSAPIImpl::registerProxy(const JSAPIImplWeakPtr &ptr) const {
    std::unique_lock<std::recursive_mutex> _l(m_proxyMutex);
    m_proxies.emplace_back(ptr);
}

void JSAPIImpl::unregisterProxy(const JSAPIImplPtr& ptr) const {
    std::unique_lock<std::recursive_mutex> _l(m_proxyMutex);
    ProxyList::iterator it = m_proxies.begin();
    while (it != m_proxies.end()) {
        JSAPIPtr cur(it->lock());
        if (!cur || ptr == cur)
            it = m_proxies.erase(it);
        else
            ++it;
    }
}
