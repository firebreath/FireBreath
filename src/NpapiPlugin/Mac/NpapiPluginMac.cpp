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

#include "NpapiTypes.h"
#include "PluginCore.h"
#include "Mac/PluginWindowMacQuickDraw.h"
#include "Mac/FactoryDefinitionsMac.h"

#include "Mac/NpapiPluginMac.h"

using namespace FB::Npapi;

NpapiPluginMac::NpapiPluginMac(FB::Npapi::NpapiBrowserHost *host) : NpapiPlugin(host), pluginWin(NULL)
{
    PluginCore::setPlatform("Mac", "NPAPI");
    // TODO: Get the path to the bundle
    //setFSPath();

    // Select CoreGraphics drawing model
    NPBool supportsCoreGraphics(false);
    if (host->GetValue(NPNVsupportsCoreGraphicsBool, &supportsCoreGraphics) == NPERR_NO_ERROR && supportsCoreGraphics) {
        host->SetValue(NPPVpluginDrawingModel, (void*)NPDrawingModelCoreGraphics);
    } else {
        throw PluginCreateError("CoreGraphics drawing model not supported; can't create plugin instance");
    }

    // For now, we default to using Carbon events
    NPBool supportsCarbonEvents(false);
    if (host->GetValue(NPNVsupportsCarbonBool, &supportsCarbonEvents) != NPERR_NO_ERROR || supportsCarbonEvents) {
        host->SetValue(NPPVpluginEventModel, (void*)NPEventModelCarbon);
    } else {
        throw PluginCreateError("Carbon event model not supported; can't create plugin instance (are you using 64 bit?)");
    }
}

NpapiPluginMac::~NpapiPluginMac()
{
    delete pluginWin; pluginWin = NULL;
}

NPError NpapiPluginMac::SetWindow(NPWindow* window)
{
    if (window != NULL && window->window != NULL) {
        NP_Port *prt = (NP_Port*)window->window;
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

int16_t NpapiPluginMac::HandleEvent(void* event)
{
    if (pluginWin != NULL)
        return pluginWin->HandleEvent((EventRecord*)event);
    else {
        return 0;
    }

}
