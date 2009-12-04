
#ifndef H_FB_PLUGINCORE
#define H_FB_PLUGINCORE

#include "PluginEventSink.h"
#include "AutoPtr.h"
#include <string>

namespace FB {

    class PluginWindow;
    class PluginEvent;
    class JSAPI;
    /** 
     * PluginCore
     *
     * This is the base class for a user defined "Plugin"
     **/
    class PluginCore : public PluginEventSink
    {
    protected:
        static volatile int ActivePluginCount;

        static std::string OS;
        static std::string Browser;

    public:
        static void setPlatform(std::string os, std::string browser);

    public:
        PluginCore();
        virtual ~PluginCore();

        void SetWindow(PluginWindow *);
        void ClearWindow();

    protected:
        virtual JSAPI* createJSAPI() = 0;

    public:
        virtual JSAPI* getRootJSAPI();
        virtual bool IsWindowless() = 0;
        virtual bool HandleEvent(PluginEvent *) = 0;

    protected:
        PluginWindow *m_Window;
        AutoPtr<JSAPI> m_api;
    };
};

#endif