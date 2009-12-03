
#ifndef H_FB_PLUGINWINDOW
#define H_FB_PLUGINWINDOW

#include <string>
#include "PluginEventSource.h"

namespace FB {

    /** 
     * PluginWindow
     *
     * This is the base class for the window of a user-defined Plugin
     * This class is responsible for firing system events to the PluginCore
     **/
    class PluginWindow : public PluginEventSource
    {
    public:
        PluginWindow() { };
        virtual ~PluginWindow() { };
    };
};

#endif