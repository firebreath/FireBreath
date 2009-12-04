/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Dec 3, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#include "NpapiTypes.h"
#include "PluginCore.h"
#include "Win/PluginWindowWin.h"
#include "Win/FactoryDefinitionsWin.h"

#include "Win/NpapiPluginWin.h"

using namespace FB::Npapi;

NpapiPluginWin::NpapiPluginWin(NpapiBrowserHost *host) : NpapiPlugin(host), pluginWin(NULL)
{
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
            pluginWin = _createPluginWindow((HWND)window->window);
            pluginMain->SetWindow(pluginWin);
        }
    } else if (pluginWin != NULL) {
        // If the handle goes to NULL, our window is gone and we need to stop using it
        pluginMain->ClearWindow();
        delete pluginWin; pluginWin = NULL;
    }

    return NPERR_NO_ERROR;
}