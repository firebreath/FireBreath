/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#ifndef H_FB_PLUGINCORE
#define H_FB_PLUGINCORE

#include "PluginEventSink.h"
#include "APITypes.h"
#include "AutoPtr.h"
#include <string>
#include <set>
#include "boost/assign.hpp"

using boost::assign::list_of;

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
        PluginCore(const std::set<std::string> params);
        virtual ~PluginCore();

        virtual void SetHost(BrowserHostWrapper *);
        virtual PluginWindow* GetWindow() const;
        virtual void SetWindow(PluginWindow *);
        virtual void ClearWindow();

    protected:
        virtual JSAPI* createJSAPI() = 0;

    public:
        virtual JSAPI* getRootJSAPI();
        virtual bool IsWindowless() = 0;
        virtual bool HandleEvent(PluginEvent *, PluginEventSource *) = 0;
        virtual void setFSPath(std::string path) { m_filesystemPath = path; }

        virtual StringSet* getSupportedParams();
        virtual void setParams(const FB::VariantMap& inParams);

    protected:
        PluginWindow *m_Window;
        AutoPtr<JSAPI> m_api;
        AutoPtr<BrowserHostWrapper> m_host;
        std::string m_filesystemPath;
        StringSet m_supportedParamSet;
        bool m_paramsSet;
        FB::VariantMap m_params;
    };
};

#endif
