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
#include "Linux/PluginWindowLinux.h"
#include "Linux/FactoryDefinitionsLinux.h"

#include "Linux/NpapiPluginLinux.h"

using namespace FB::Npapi;

NpapiPluginLinux::NpapiPluginLinux(FB::Npapi::NpapiBrowserHost *host) : NpapiPlugin(host), pluginWin(NULL)
{
    // TODO: Get the path to the shared lib
    //setFSPath();

}

NpapiPluginLinux::~NpapiPluginLinux()
{
    delete pluginWin; pluginWin = NULL;
}

NPError NpapiPluginLinux::SetWindow(NPWindow* window)
{
    if (window != NULL && window->window != NULL) {

        if (pluginWin != NULL && pluginWin->getPort() != prt->port) {
            pluginMain->ClearWindow();
            delete pluginWin; pluginWin = NULL;
        }

        if (pluginWin == NULL) {
            pluginWin = _createPluginWindow((CGrafPtr)prt->port, prt->portx, prt->porty);
            pluginMain->SetWindow(pluginWin);
        }

        pluginWin->setWindowPosition(window->x, window->y, window->width, window->height);
        pluginWin->setWindowClipping(window->clipRect.top, window->clipRect.left,
                                      window->clipRect.bottom, window->clipRect.right);
    } else if (pluginWin != NULL) {
        // If the handle goes to NULL, our window is gone and we need to stop using it
        pluginMain->ClearWindow();
        delete pluginWin; pluginWin = NULL;
    }

    return NPERR_NO_ERROR;
}

