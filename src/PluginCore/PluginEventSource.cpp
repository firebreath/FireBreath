
#include "PluginEvent.h"
#include "PluginEventSink.h"
#include "PluginEventSource.h"
#include "PluginEvents/AttachedEvent.h"

using namespace FB;

PluginEventSource::PluginEventSource()
{
}

PluginEventSource::~PluginEventSource()
{
}

void PluginEventSource::AttachObserver(FB::PluginEventSink *sink)
{
    m_observers.push_back(sink);
    AttachedEvent newEvent(true);
    sink->HandleEvent(&newEvent);
}

void PluginEventSource::DetachObserver(FB::PluginEventSink *sink)
{
    for (ObserverMap::iterator it = m_observers.begin(); it != m_observers.end(); it++) {
        if (*it == sink) {
            m_observers.erase(it);
            AttachedEvent newEvent(false);
            sink->HandleEvent(&newEvent);
            return;
        }
    }
}

bool PluginEventSource::SendEvent(PluginEvent* evt)
{
    for (ObserverMap::iterator it = m_observers.begin(); it != m_observers.end(); it++) {
        if ((*it)->HandleEvent(evt)) {
            return true;    // Tell the caller that the event was handled
        }
    }
    return false;
}