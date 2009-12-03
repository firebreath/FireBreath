
#ifndef H_FB_EVENTS_WINDOWATTACHEDEVENT
#define H_FB_EVENTS_WINDOWATTACHEDEVENT

#include "PluginEvent.h"

namespace FB {

    class AttachedEvent : public PluginEvent
    {
    public:
        AttachedEvent(bool status) : m_status(status) { }
        bool getStatus() { return m_status; }

    protected:
        bool m_status;
    };
};

#endif