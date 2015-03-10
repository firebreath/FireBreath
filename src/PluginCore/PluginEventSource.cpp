/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Nov 24, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#include <algorithm>
#include "PluginEvent.h"
#include "PluginEventSink.h"
#include "PluginEventSource.h"
#include "PluginEvents/AttachedEvent.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

using namespace FB;

PluginEventSource::PluginEventSource()
{
}

PluginEventSource::~PluginEventSource()
{
}

void PluginEventSource::AttachObserver(FB::PluginEventSink *sink)
{
    AttachObserver(sink->shared_from_this());
}

void PluginEventSource::AttachObserver( PluginEventSinkPtr sink )
{
    std::unique_lock<std::recursive_mutex> _l(m_observerLock);
    m_observers.emplace_back(sink);
    AttachedEvent newEvent;
    sink->HandleEvent(&newEvent, this);
}

void PluginEventSource::DetachObserver(FB::PluginEventSink *sink)
{
    DetachObserver(sink->shared_from_this());
}

void PluginEventSource::DetachObserver( PluginEventSinkPtr sink )
{
    std::unique_lock<std::recursive_mutex> _l(m_observerLock);
	
	std::list<PluginEventSinkPtr> detachedList;
	{
		auto end = m_observers.end();
		for (auto it = m_observers.begin(); it != end; ) {
			PluginEventSinkPtr ptr(it->lock());
			if (!ptr || sink == ptr) {
				it = m_observers.erase(it);
				if (ptr)
					detachedList.emplace_back(ptr);
			} else {
				++it;
			}
		}
	}
	
	DetachedEvent evt;
    for (auto cur : detachedList) {
        cur->HandleEvent(&evt, this);
    }
}

bool PluginEventSource::SendEvent(PluginEvent* evt)
{
    std::unique_lock<std::recursive_mutex> _l(m_observerLock);

    // Sometimes the events cause an observer to be removed; we make a copy so that 
    // it doesn't mess with our iterator.  Remember that removing an observer will only take
    // affect on the next SendEvent call
    ObserverMap copy(m_observers);
    for (auto cur : copy) {
        PluginEventSinkPtr tmp = cur.lock();
        if (tmp && tmp->HandleEvent(evt, this)) {
            return true;    // Tell the caller that the event was handled
        }
    }
    return false;
}

