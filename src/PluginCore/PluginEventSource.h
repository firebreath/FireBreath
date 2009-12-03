
#ifndef H_FB_PLUGINEVENTSOURCE
#define H_FB_PLUGINEVENTSOURCE

#include <vector>

namespace FB {

    class PluginEventSink;
    class PluginEvent;

    /** 
     * PluginWindow
     *
     * This is the base class for the window of a user-defined Plugin
     **/
    class PluginEventSource
    {
    public:
        PluginEventSource();
        virtual ~PluginEventSource();

        virtual void AttachObserver(PluginEventSink*);
        virtual void DetachObserver(PluginEventSink*);
        virtual bool SendEvent(PluginEvent*);

    protected:
        typedef std::vector<PluginEventSink*> ObserverMap;
        ObserverMap m_observers;
    };
};

#endif