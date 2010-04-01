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
#include "Mac/PluginWindowMacQuickDraw.h"
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
        
        err = host->GetValue(what, &value);
        
        if (err != NPERR_NO_ERROR) {
            return false;
        }
        
        return value;
    }
    
    bool set(FB::Npapi::NpapiBrowserHost* host, NPPVariable what, void* value)
    {
        NPError err = host->SetValue(what, value);
        return (err == NPERR_NO_ERROR);
    }
    
    bool enableQuickDraw(FB::Npapi::NpapiBrowserHost* host)
    {
#if !FBMAC_USE_QUICKDRAW
        return false;  
#else
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

    bool enableCoreGraphics(FB::Npapi::NpapiBrowserHost* host)
    {
#if !FBMAC_USE_COREGRAPHICS
        return false;
#else
        if(!supports(host, NPNVsupportsCocoaBool))
            return false;           
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
    } else if(enableCoreGraphics(host)) {
        m_eventModel   = EventModelCocoa;
        m_drawingModel = DrawingModelCoreGraphics;
    } else if(enableQuickDraw(host)) {
        m_eventModel   = EventModelCarbon;
        m_drawingModel = DrawingModelQuickDraw;
    } else {
        throw PluginCreateError("none of the chosen drawing and event models are supported");
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

NPError NpapiPluginMac::SetWindowQuickDraw(NPWindow* window)
{
#if !FBMAC_USE_QUICKDRAW
    return NPERR_GENERIC_ERROR;
#else
    PluginWindowMacQuickDraw* win = static_cast<PluginWindowMacQuickDraw*>(pluginWin);
    
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

NPError NpapiPluginMac::SetWindowCoreGraphics(NPWindow* window)
{
#if !FBMAC_USE_COREGRAPHICS
    return NPERR_GENERIC_ERROR;
#else
    return NPERR_NO_ERROR;
#endif
}

NPError NpapiPluginMac::SetWindowCoreAnimation(NPWindow* window)
{
#if !FBMAC_USE_COREGRAPHICS
    return NPERR_GENERIC_ERROR;
#else
    return NPERR_NO_ERROR;
#endif    
}

NPError NpapiPluginMac::SetWindow(NPWindow* window)
{
    switch(m_drawingModel)
    {
        case DrawingModelQuickDraw:
            return SetWindowQuickDraw(window);
        case DrawingModelCoreGraphics:
            return SetWindowCoreGraphics(window);
        case DrawingModelCoreAnimation:
            return SetWindowCoreAnimation(window);
    }

    return NPERR_GENERIC_ERROR;
}

int16_t NpapiPluginMac::HandleEventCarbon(void* event)
{
#if !FBMAC_USE_CARBON
    return 0;
#else
    if (!pluginWin) {
        return 0;
    }
     
    PluginWindowMacQuickDraw* win = static_cast<PluginWindowMacQuickDraw*>(pluginWin);
    return win->HandleEvent((EventRecord*)event);
#endif    
}

int16_t NpapiPluginMac::HandleEventCocoa(void* event)
{
#if !FBMAC_USE_COCOA
    return 0;
#else
    return 0;
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
