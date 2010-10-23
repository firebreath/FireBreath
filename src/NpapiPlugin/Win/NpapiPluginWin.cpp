/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Dec 3, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#include "Win/win_common.h"
#include "NpapiTypes.h"
#include "PluginCore.h"
#include "Win/PluginWindowWin.h"
#include "FactoryBase.h"

#include "Win/NpapiPluginWin.h"
#include "NpapiPluginFactory.h"
#include <boost/make_shared.hpp>

using namespace FB::Npapi;

extern std::string g_dllPath;

FB::Npapi::NpapiPluginPtr FB::Npapi::createNpapiPlugin(const FB::Npapi::NpapiBrowserHostPtr& host)
{
    return boost::make_shared<NpapiPluginWin>(host);
}

NpapiPluginWin::NpapiPluginWin(const NpapiBrowserHostPtr& host) : NpapiPlugin(host), pluginWin(NULL)
{
    PluginCore::setPlatform("Windows", "NPAPI");
    setFSPath(g_dllPath);
}

NpapiPluginWin::~NpapiPluginWin()
{
    delete pluginWin; pluginWin = NULL;
}

NPError NpapiPluginWin::SetWindow(NPWindow* window)
{
    if (window != NULL) {
        if (pluginWin != NULL
             && pluginWin->getHWND() != window->window) {

            pluginMain->ClearWindow();
            delete pluginWin; pluginWin = NULL;
        }

        if (pluginWin == NULL) {
            HWND browserHWND;
            m_npHost->GetValue(NPNVnetscapeWindow, (void*)&browserHWND);

            pluginWin = getFactoryInstance()->createPluginWindowWin(FB::WindowContextWin((HWND)window->window));
            pluginWin->setBrowserHWND(browserHWND);
            pluginMain->SetWindow(pluginWin);
            setReady();
        }
    } else if (pluginWin != NULL) {
        // If the handle goes to NULL, our window is gone and we need to stop using it
        pluginMain->ClearWindow();
        delete pluginWin; pluginWin = NULL;
    }

    return NPERR_NO_ERROR;
}
