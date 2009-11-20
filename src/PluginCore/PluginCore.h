
#ifndef H_FB_PLUGINCORE
#define H_FB_PLUGINCORE

#include "PluginEventSink.h"
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

    protected:
        // These are pseudo-static.  They aren't really static, but it isn't
        // safe to assume that the instance that Initialize is called on will
        // be the same as the instance that Deinitialize is called on
        virtual void StaticInitialize() = 0;
        virtual void StaticDeinitialize() = 0;

    public:
        PluginCore();
        virtual ~PluginCore();

        void SetWindow(PluginWindow *);
        void ClearWindow();

    public:
        virtual JSAPI* getRootJSAPI() = 0;
        virtual bool IsWindowless() = 0;
        virtual bool HandleEvent(PluginEvent *) = 0;

    protected:
        PluginWindow *m_Window;
    };
};

#endif