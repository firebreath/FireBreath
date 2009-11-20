
#ifndef H_FB_PLUGINEVENTSINK
#define H_FB_PLUGINEVENTSINK

#include <string>

namespace FB {

    class PluginEvent;
    /** 
     * PluginWindow
     *
     * This is the base class for the window of a user-defined Plugin
     **/
    class PluginEventSink
    {
    public:
        PluginEventSink() { };
        virtual ~PluginEventSink() { };

    public:
        virtual bool HandleEvent(PluginEvent *) = 0;
    };
};

#endif