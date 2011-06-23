/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Jun 23, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2011 Facebook, Inc
\**********************************************************/
#pragma once
#ifndef WebView_h__
#define WebView_h__

#include <boost/function.hpp>
#include "PluginEventSource.h"
#include "PluginWindow.h"
#include "APITypes.h"
#include "BrowserHost.h"
#include "PluginCore.h"
#include "URI.h"
#include "DOM.h"

namespace FB {namespace View {
    FB_FORWARD_PTR(WebView);
    class WebView : public FB::PluginEventSink
    {
        enum WebEvent {
            WebEvent_DocumentLoaded
        };
        typedef boost::function<void (WebView*, WebEvent)> WebEventCallback;
    public:
        WebView(const FB::PluginCorePtr& plugin, const FB::BrowserHostPtr& parentHost)
            : m_parentBrowser(parentHost), m_plugin(plugin) {}
        virtual ~WebView() {}

        static WebViewPtr create(const FB::PluginCorePtr& plugin, const FB::BrowserHostPtr& parentHost);
        virtual void init() {
            FB::PluginWindow* win(getPlugin()->GetWindow());
            if (win)
                win->AttachObserver(shared_from_this());
            // If you initialize this or might initialize this before the window is attached to the plugin
            // you can call this init method from your attachedevent; it is safe to call multiple times
        }

    public:
        // Page Management
        virtual void loadUri(const FB::URI& uri) = 0;
        virtual void loadHtml(const std::string& html) = 0;
        virtual void closePage() = 0;

        // Accessing the page
        virtual FB::BrowserHostPtr getPageHost() = 0;

    public:
        // Exposing information to the page
        virtual void setJSVar(const std::string& name, const FB::variant& val) {
            m_jsVariables[name] = val;
        }
        virtual FB::VariantMap& getJSVariables() { return m_jsVariables; }

    protected:
        FB::BrowserHostPtr getParentHost() {
            return FB::BrowserHostPtr(m_parentBrowser);
        }
        FB::PluginCorePtr getPlugin() {
            return FB::PluginCorePtr(m_plugin);
        }

    private:
        FB::BrowserHostWeakPtr m_parentBrowser;
        FB::PluginCoreWeakPtr m_plugin;
        FB::VariantMap m_jsVariables;
    };

}}
#endif // WebView_h__
