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
#include "Mac/PluginWindowMacCarbonQD.h"
#include "Mac/PluginWindowMacCarbonCG.h"
#include "Mac/PluginWindowMacCocoaCA.h"
#include "Mac/FactoryDefinitionsMac.h"

#include "Mac/NpapiPluginMac.h"
#include "config.h"

using namespace FB::Npapi;

namespace 
{
    bool supports(FB::Npapi::NpapiBrowserHost* host, NPNVariable what)
    {
        NPBool value;        
        NPError err;
        
        // err tells us if the browser supports model negotiation
        err = host->GetValue(what, &value);
        
        if (err != NPERR_NO_ERROR) {
#ifndef __LP64__
            if (what == NPNVsupportsCarbonBool || what == NPNVsupportsQuickDrawBool) {
                // Model negotiation is not supported, assume
                // that Carbon and QuickDraw are supported & return true;
                return true;
            }
#endif

            // If we aren't testing for Carbon or QD, assume that
            // an NPERR_GENERIC_ERROR is bad, return unsupported
            return false;
        }
        
        // If GetValue returns NPERR_NO_ERROR then return whether or not
        // the browser supports the indicated Drawing/Event model
        return value;
    }

    bool set(FB::Npapi::NpapiBrowserHost* host, NPPVariable what, void* value)
    {
        NPError err = host->SetValue(what, value);
        void* model = value;

        if(err != NPERR_NO_ERROR) {
#ifndef __LP64__
            if(model == (void*)NPEventModelCarbon || model == (void*)NPDrawingModelQuickDraw) {
                // Model negotiation is not supported, assume
                // that Carbon and QuickDraw are supported & return true;
                return true;
            }
#endif

            // If we aren't testing for Carbon or QD, assume that
            // an NPERR_GENERIC_ERROR is bad, return unsupported
            return false;
        }

        // If SetValue returns NPERR_NO_ERROR then SetValue was successful
        return true;
    }
    
    bool enableQuickDraw(FB::Npapi::NpapiBrowserHost* host)
    {
#if !FBMAC_USE_QUICKDRAW
        printf("enableQuickDraw() - 0\n");
        return false;  
#else
        printf("enableQuickDraw() - 1\n");
        
        // QuickDraw can only be used with Carbon Event Model
        if(!supports(host, NPNVsupportsCarbonBool))
            return false;           
        if(!supports(host, NPNVsupportsQuickDrawBool))
            return false;
        if(!set(host, NPPVpluginEventModel, (void*)NPEventModelCarbon))
            return false;
        if(!set(host, NPPVpluginDrawingModel, (void*)NPDrawingModelQuickDraw))
            return false;
        
        return true;
#endif   
    }

    bool enableCoreGraphicsCarbon(FB::Npapi::NpapiBrowserHost* host)
    {
#if !FBMAC_USE_COREGRAPHICS
        printf("CoreGraphics not supported\n");
        return false;
#endif
#if !FBMAC_USE_CARBON
        printf("Carbon not supported\n");
        return false;
#else
        
        if(!supports(host, NPNVsupportsCarbonBool))
            return false;
        if(!supports(host, NPNVsupportsCoreGraphicsBool))
            return false;
        if(!set(host, NPPVpluginEventModel, (void*)NPEventModelCarbon))
            return false;
        if(!set(host, NPPVpluginDrawingModel, (void*)NPDrawingModelCoreGraphics))
            return false;
        
        printf("CoreGraphics and Carbon supported\n");
        return true;
#endif
    }

    bool enableCoreGraphicsCocoa(FB::Npapi::NpapiBrowserHost* host)
    {
#if !FBMAC_USE_COREGRAPHICS
        printf("CoreGraphics not supported\n");
        return false;
#endif
#if !FBMAC_USE_CARBON
        printf("Cocoa not supported\n");
        return false;
#else
        printf("CoreGraphics and Cocoa supported\n");
        
        if(!supports(host, NPNVsupportsCocoaBool)) {
            return false;           
        }
        if(!supports(host, NPNVsupportsCoreGraphicsBool))
            return false;
        if(!set(host, NPPVpluginEventModel, (void*)NPEventModelCocoa))
            return false;
        if(!set(host, NPPVpluginDrawingModel, (void*)NPDrawingModelCoreGraphics))
            return false;
        
        return true;
#endif
    }
    
    bool enableCoreAnimation(FB::Npapi::NpapiBrowserHost* host)
    {
#if !FBMAC_USE_COREANIMATION
        printf("enableCoreAnimation() - 0\n");
        return false;  
#else
        printf("enableCoreAnimation() - 1\n");
        
        if(!supports(host, NPNVsupportsCocoaBool))
           return false;           
        if(!supports(host, NPNVsupportsCoreAnimationBool))
           return false;
        if(!set(host, NPPVpluginEventModel, (void*)NPEventModelCocoa))
            return false;
        if(!set(host, NPPVpluginDrawingModel, (void*)NPDrawingModelCoreAnimation))
            return false;
        
        return true;
#endif
    }
}

NpapiPluginMac::NpapiPluginMac(FB::Npapi::NpapiBrowserHost *host)
  : NpapiPlugin(host)   
  , pluginWin(NULL)
  , m_eventModel()
  , m_drawingModel()
{
    PluginCore::setPlatform("Mac", "NPAPI");
    // TODO: Get the path to the bundle
    //setFSPath();
    
    if(enableCoreAnimation(host)) {
        m_eventModel   = EventModelCocoa;
        m_drawingModel = DrawingModelCoreAnimation;
    } else if(enableCoreGraphicsCarbon(host)) {
        m_eventModel   = EventModelCarbon;
        m_drawingModel = DrawingModelCoreGraphics;
    } else if(enableCoreGraphicsCocoa(host)) {
        m_eventModel   = EventModelCocoa;
        m_drawingModel = DrawingModelCoreGraphics;
    } else if(enableQuickDraw(host)) {
        m_eventModel   = EventModelCarbon;
        m_drawingModel = DrawingModelQuickDraw;
    } else {
#ifdef __LP64__
        // Default to Cocoa/CG for 64 bit plugins
        m_eventModel = EventModelCocoa;
        m_drawingModel = DrawingModelCoreGraphics;
#else
        // Default to Carbon/QD for 32 bit plugins
        m_eventModel = EventModelCarbon;
        m_drawingModel = DrawingModelQuickDraw;
#endif
    }
}

NpapiPluginMac::~NpapiPluginMac()
{
    delete pluginWin; pluginWin = NULL;
}

NpapiPluginMac::EventModel NpapiPluginMac::getEventModel() const
{
    return m_eventModel;
}

NpapiPluginMac::DrawingModel NpapiPluginMac::getDrawingModel() const
{
    return m_drawingModel;
}

NPError NpapiPluginMac::SetWindowCarbonQD(NPWindow* window)
{
#if !FBMAC_USE_QUICKDRAW
    return NPERR_GENERIC_ERROR;
#else
    PluginWindowMacCarbonQD* win = static_cast<PluginWindowMacCarbonQD*>(pluginWin);
    
    if (window != NULL && window->window != NULL) 
    {
        NP_Port *prt = (NP_Port*)window->window;
        
        if ((win != NULL) && (win->getPort() != prt->port))
        {
            pluginMain->ClearWindow();
            delete pluginWin;
            pluginWin = NULL;
        }
        
        if (pluginWin == NULL) 
        {
            win = _createPluginWindow((CGrafPtr)prt->port, prt->portx, prt->porty);
            pluginWin = static_cast<PluginWindow*>(win);
            pluginMain->SetWindow(win);
        }
        
        win->setWindowPosition(window->x, window->y, window->width, window->height);
        win->setWindowClipping(window->clipRect.top,    window->clipRect.left,
                               window->clipRect.bottom, window->clipRect.right);
    }
    else if (pluginWin != NULL) 
    {
        // If the handle goes to NULL, our window is gone and we need to stop using it
        pluginMain->ClearWindow();
        delete pluginWin;
        pluginWin = NULL;
    }
    
    return NPERR_NO_ERROR;
#endif
}

NPError NpapiPluginMac::SetWindowCarbonCG(NPWindow* window) {
#if !FBMAC_USE_COREGRAPHICS
    return NPERR_GENERIC_ERROR;
#else

    // SetWindow provides us with the window that our plugin should draw to.
    // In the Cocoa event model the window.window is null in the passed NPWindow

    PluginWindowMacCarbonCG* pluginWinCG = static_cast<PluginWindowMacCarbonCG*>(pluginWin);

    if (window != NULL) {
        if (pluginWin != NULL) {
            if (pluginWinCG->getContext() != (NP_CGContext*)window->window) {
                pluginMain->ClearWindow(); // Received new window, kill the old one
                delete pluginWin;
                pluginWin = NULL;
            } else {
                // Received old window
                return NPERR_NO_ERROR;
            }
        }

        if (pluginWin == NULL) {
            // We have no plugin window associated with this plugin object.
            // Make a new plugin window object for FireBreath & our plugin.
            pluginWinCG = _createPluginWindow((NP_CGContext*)window->window);
            pluginWin = static_cast<PluginWindow*>(pluginWinCG);
        }

        // Initialize the window position & clipping from the newly arrived NPWindow window
        pluginWinCG->setWindowPosition(window->x, window->y, window->width, window->height);
        pluginWinCG->setWindowClipping(window->clipRect.top, window->clipRect.left, 
                                     window->clipRect.bottom, window->clipRect.right);
        
        pluginMain->SetWindow(pluginWin);
    } else if (pluginWin != NULL) {
        // Our window is gone, we should stop using it
        pluginMain->ClearWindow();
        delete pluginWin;
        pluginWin = NULL;
    } else {
        return NPERR_GENERIC_ERROR;
    }

    return NPERR_NO_ERROR;
#endif
}

NPError NpapiPluginMac::SetWindowCocoaCG(NPWindow* window) {
    // TODO: Implement
    return NPERR_GENERIC_ERROR;
}

NPError NpapiPluginMac::SetWindowCocoaCA(NPWindow* window) {
#if !FBMAC_USE_COREANIMATION
    return NPERR_GENERIC_ERROR;
#else

    PluginWindowMacCocoaCA* pluginWinCA = static_cast<PluginWindowMacCocoaCA*>(pluginWin);
    
    if (window != NULL) {
        if (pluginWin != NULL) {
            pluginMain->ClearWindow(); // Kill old window
            delete pluginWin;
            pluginWin = NULL;
        }

        if (pluginWin = NULL) {
            // No window associated with this plugin object.
            // Make a new window and associate with the object.
            pluginWinCA = _createPluginWindow();
            pluginWin = static_cast<PluginWindow*>(pluginWinCA);
            pluginMain->SetWindow(pluginWin);
        }

        // Initialize window position & clipping 
        pluginWinCA->setWindowPosition(window->x, window->y, window->width, window->height);
        pluginWinCA->setWindowClipping(window->clipRect.top, window->clipRect.left, 
                                    window->clipRect.bottom, window->clipRect.right);
    } else if (pluginWin != NULL) {
        // Our window is gone, we should stop using it
        pluginMain->ClearWindow();
        delete pluginWin;
        pluginWin = NULL;
    }

    return NPERR_NO_ERROR;
#endif    
}

NPError NpapiPluginMac::SetWindow(NPWindow* window) {
    switch(m_drawingModel)
    {
        case DrawingModelQuickDraw:
            return SetWindowCarbonQD(window);
        case DrawingModelCoreGraphics:
            {
                return SetWindowCarbonCG(window);
            }
        case DrawingModelCoreAnimation:
            {
                return SetWindowCocoaCA(window);
            }
    }

    return NPERR_GENERIC_ERROR;
}

int16_t NpapiPluginMac::HandleEventCarbon(void* event) {
#if !FBMAC_USE_CARBON
    return 0;
#else
    if (!pluginWin) {
        return 0;
    }
    
    PluginWindowMacCarbonCG* win = static_cast<PluginWindowMacCarbonCG*>(pluginWin);
    return win->HandleEvent((EventRecord*)event);
#endif
}

int16_t NpapiPluginMac::HandleEventCocoa(void* event)
{
#if !FBMAC_USE_COCOA
    return 0;
#else
    if (!pluginWin) {
        return 0;
    }

    PluginWindowMacCocoa* win = static_cast<PluginWindowMacCocoa*>(pluginWin);
    return win->HandleEvent((NPCocoaEvent*)event);
#endif
}

int16_t NpapiPluginMac::HandleEvent(void* event)
{
    switch(m_eventModel)
    {
        case EventModelCarbon:
            return HandleEventCarbon(event);
        case EventModelCocoa:
            return HandleEventCocoa(event);
    }
    
    return 0;
}
