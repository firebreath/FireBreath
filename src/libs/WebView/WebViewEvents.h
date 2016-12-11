/**********************************************************\ 
Original Author: Garry Bodsworth

Created:    Apr 26, 2013
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2013 the Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_WEBVIEW_EVENTS
#define H_FB_WEBVIEW_EVENTS

#include "PluginEvent.h"

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  WebViewNavigation
    ///
    /// @brief  Fired when a navigation event is received.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class WebViewNavigation : public PluginEvent
    {
    public:
        WebViewNavigation(const std::string& url)
            : m_url(url)
        { }

    public:
        std::string m_url;   // The url being navigated to.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  WebViewTitleChanged
    ///
    /// @brief  Fired when the WebView title changes.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class WebViewTitleChanged : public PluginEvent
    {
    public:
        WebViewTitleChanged(const std::string& title)
            : m_title(title)
        { }

    public:
        std::string m_title;   // The new title.
    };
};

#endif
