/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#ifndef H_FB_PLUGINCORE
#define H_FB_PLUGINCORE

#include "PluginEventSink.h"
#include "AutoPtr.h"
#include <string>

namespace FB {

    class PluginWindow;
    class PluginEvent;
    class JSAPI;
    class BrowserHostWrapper;
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

        void SetHost(BrowserHostWrapper *);
		PluginWindow* GetWindow();
        void SetWindow(PluginWindow *);
        void ClearWindow();

    protected:
        virtual JSAPI* createJSAPI() = 0;

    public:
        virtual JSAPI* getRootJSAPI();
        virtual bool IsWindowless() = 0;
        virtual bool HandleEvent(PluginEvent *, PluginEventSource *) = 0;

    protected:
        PluginWindow *m_Window;
        AutoPtr<JSAPI> m_api;
        AutoPtr<BrowserHostWrapper> m_host;
    };
};

#endif