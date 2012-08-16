/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_BROWSERPLUGIN
#define H_FB_BROWSERPLUGIN

#include "APITypes.h"
#include "JSAPI.h"
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace FB {

    FB_FORWARD_PTR(PluginCore);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  BrowserPlugin
    ///
    /// @brief  Browser-specific plugin base class
    ///
    /// This object is the base class for FBControl and NpapiPlugin, which are the browser-specific
    /// plugin classes that contain a PluginCore derived plugin object.
    ///
    /// Unless you are working on the core you shouldn't need to worry about this.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class BrowserPlugin : boost::noncopyable
    {
    public:
        BrowserPlugin(const std::string& mimetype);
        virtual ~BrowserPlugin();

        virtual void shutdown() = 0;
        virtual void setFSPath(const std::string& path);

        static std::string getFSPath() { return fileSystemPath; }

    protected:
        // Even though this is a shared pointer, don't ever hold onto a reference to it except
        // as a weak_ptr, and then don't keep it locked longer than needed.
        FB::PluginCorePtr pluginMain;

        static std::string fileSystemPath;
    };

};
#endif

