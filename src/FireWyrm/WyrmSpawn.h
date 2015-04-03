/**********************************************************\
 Original Author: Richard Bateman (taxilian)

 Created:    Jan 19, 2015
 License:    Dual license model; choose one of two:
             New BSD License
             http://www.opensource.org/licenses/bsd-license.php
             - or -
             GNU Lesser General Public License, version 2.1
             http://www.gnu.org/licenses/lgpl-2.1.html

 Copyright 2015 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_FIREWYRM_FIREWYRMPLUGIN
#define H_FB_FIREWYRM_FIREWYRMPLUGIN

#include "WyrmBrowserHost.h"
#include "BrowserPlugin.h"

namespace FB {
    FB_FORWARD_PTR(BrowserStream);

    namespace FireWyrm {

    struct WyrmSpawningError : std::runtime_error
    {
        WyrmSpawningError(const std::string& error)
          : std::runtime_error(error)
        { }
    };

    FB_FORWARD_PTR(WyrmSpawn);
    FB_FORWARD_PTR(WyrmBrowserHost);

    class WyrmSpawn : public FB::BrowserPlugin
    {
    public:
        WyrmSpawn(WyrmBrowserHostPtr host, std::string mimetype);
        virtual ~WyrmSpawn(void);

        virtual void init(VariantMap args);
        virtual void shutdown();

        virtual bool setReady(Promise<void> onReady);

    protected:
        WyrmBrowserHostPtr m_fwHost;
        bool m_isReady;
        std::string m_mimetype;
        std::string m_pluginDesc;
        std::string m_pluginName;
    };

}; }; // FB::FireWyrm

#endif // H_FB_FIREWYRM_FIREWYRMPLUGIN


