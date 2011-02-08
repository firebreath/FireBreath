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

#include <stdio.h>
#include <stdlib.h>

#include <dlfcn.h>

#include "NpapiBrowserHost.h"
#include "PluginCore.h"
#include "global/config.h"

#if FBMAC_USE_COREANIMATION
#include <QuartzCore/CoreAnimation.h>
#endif

#include "FactoryBase.h"

#include "Mac/PluginWindowMacCarbonQD.h"
#include "Mac/PluginWindowMacCarbonCG.h"
#include "Mac/PluginWindowMacCocoa.h"
#include "Mac/PluginWindowMacCocoaCG.h"
#include "Mac/PluginWindowMacCocoaCA.h"
#include "Mac/PluginWindowMacCocoaICA.h"

#include "NpapiPluginFactory.h"
#include <boost/make_shared.hpp>
#include <string>

#include "Mac/NpapiPluginMac.h"

using namespace FB::Npapi;

FB::Npapi::NpapiPluginPtr FB::Npapi::createNpapiPlugin(const FB::Npapi::NpapiBrowserHostPtr& host, const std::string& mimetype)
{
    return boost::make_shared<NpapiPluginMac>(host, mimetype);
}

namespace {
    bool supports(const FB::Npapi::NpapiBrowserHostPtr &host, NPNVariable what) {
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
#else
            if (what == NPNVsupportsCocoaBool || what == NPNVsupportsCoreGraphicsBool) {
                // Model negotiation is not supported, assume
                // that Cocoa and CoreGraphics are supported & return true;
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

    bool set(const FB::Npapi::NpapiBrowserHostPtr &host, NPPVariable what, void* value) {
        NPError err = host->SetValue(what, value);
        void* model = value;

        if(err != NPERR_NO_ERROR) {
#ifndef __LP64__
            if(model == (void*)NPEventModelCarbon || model == (void*)NPDrawingModelQuickDraw) {
                // Model negotiation is not supported, assume
                // that Carbon and QuickDraw are supported & return true;
                return true;
            }
#else
            if(model == (void*)NPEventModelCocoa || model == (void*)NPDrawingModelCoreGraphics) {
                // Model negotiation is not supported, assume
                // that Cocoa and CoreGraphics are supported & return true;
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
    
    bool enableQuickDraw(const FB::Npapi::NpapiBrowserHostPtr &host) {
#ifdef __LP64__
        // QuickDraw does not exist for 64 bit 
        return false;
#else

#if !FBMAC_USE_QUICKDRAW
        printf("enableQuickDraw() - 0\n");
        return false;  
#else
        printf("enableQuickDraw() - 1\n");
        
        // QuickDraw can only be used with Carbon Event Model
#ifndef __LP64__
        if(!supports(host, NPNVsupportsCarbonBool))
            return false;           
        if(!supports(host, NPNVsupportsQuickDrawBool))
            return false;
        if(!set(host, NPPVpluginEventModel, (void*)NPEventModelCarbon))
            return false;
        if(!set(host, NPPVpluginDrawingModel, (void*)NPDrawingModelQuickDraw))
            return false;
#endif
#endif
        return true;
#endif   
    }

    bool enableCoreGraphicsCarbon(const FB::Npapi::NpapiBrowserHostPtr &host) {
#ifdef __LP64__
        // Carbon does not exist for 64 bit
        return false;
#else

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
#endif
    }

    bool enableCoreGraphicsCocoa(const FB::Npapi::NpapiBrowserHostPtr &host) {
#if !FBMAC_USE_COREGRAPHICS
        printf("CoreGraphics not supported\n");
        return false;
#endif
#if !FBMAC_USE_COCOA
        printf("Cocoa not supported\n");
        return false;
#else
        printf("CoreGraphics and Cocoa supported\n");
        
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
    
    bool enableCoreAnimation(const FB::Npapi::NpapiBrowserHostPtr &host) {
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

    bool enableInvalidatingCoreAnimation(const FB::Npapi::NpapiBrowserHostPtr &host) {
#if !FBMAC_USE_COREANIMATION
        printf("enableCoreAnimation() - 0\n");
        return false;  
#else
        printf("enableCoreAnimation() - 1\n");
        
        if(!supports(host, NPNVsupportsCocoaBool))
            return false;
        if(!supports(host, NPNVsupportsInvalidatingCoreAnimationBool))
            return false;
        if(!set(host, NPPVpluginEventModel, (void*)NPEventModelCocoa))
            return false;
        if(!set(host, NPPVpluginDrawingModel, (void*)NPDrawingModelInvalidatingCoreAnimation))
            return false;
        
        return true;
#endif
    }
}

namespace 
{
    std::string getPluginPath() 
    {
        ::Dl_info dlinfo; 
        if (::dladdr((void*)::NP_Initialize, &dlinfo) != 0) { 
            return dlinfo.dli_fname; 
        } else {
            return "";
        }
    }
}

NpapiPluginMac::NpapiPluginMac(const FB::Npapi::NpapiBrowserHostPtr &host, const std::string& mimetype)
  : NpapiPlugin(host, mimetype), pluginWin(NULL), m_eventModel(), m_drawingModel() {
    // If you receive a BAD_ACCESS error here you probably have something
    // wrong in your FactoryMain.cpp in your plugin project's folder

    PluginCore::setPlatform("Mac", "NPAPI");
    
    // Get the path to the bundle
    static const std::string pluginPath = getPluginPath();      
    setFSPath(pluginPath);

    if(enableInvalidatingCoreAnimation(host)) {
        m_eventModel = EventModelCocoa;
        m_drawingModel = DrawingModelInvalidatingCoreAnimation;
#if FBMAC_USE_COCOA && FBMAC_USE_COREANIMATION
        // This hack exists to allow the plugin to call setLayer() on 
        // the newly created PluginWindowMacCocoaCA window. This must
        // be done before SetWindowCA() since the browser will call
        // GetValue() for the CALayer before it calls SetWindow.
        PluginWindowMacCocoaICA* pluginWinICA = getFactoryInstance()->createPluginWindowCocoaICA();
        pluginWinICA->setNpHost(m_npHost);
        this->pluginWin = static_cast<PluginWindow*>(pluginWinICA);
        pluginMain->SetWindow(pluginWinICA);
#endif
    } else if(enableCoreAnimation(host)) {
        m_eventModel   = EventModelCocoa;
        m_drawingModel = DrawingModelCoreAnimation;
#if FBMAC_USE_COCOA && FBMAC_USE_COREANIMATION
        // This hack exists to allow the plugin to call setLayer() on 
        // the newly created PluginWindowMacCocoaCA window. This must
        // be done before SetWindowCA() since the browser will call
        // GetValue() for the CALayer before it calls SetWindow.
        PluginWindowMacCocoaCA* pluginWinCA = getFactoryInstance()->createPluginWindowCocoaCA();
        pluginWinCA->setNpHost(m_npHost);
        this->pluginWin = static_cast<PluginWindow*>(pluginWinCA);
        pluginMain->SetWindow(pluginWinCA);
#endif
    } else if(enableCoreGraphicsCocoa(host)) {
        m_eventModel   = EventModelCocoa;
        m_drawingModel = DrawingModelCoreGraphics;
    } else if(enableCoreGraphicsCarbon(host)) {
        m_eventModel   = EventModelCarbon;
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
#if FBMAC_USE_COCOA && FBMAC_USE_COREANIMATION
    PluginWindowMacCocoaCA* win = dynamic_cast<PluginWindowMacCocoaCA*>(pluginWin);
    if(win != NULL) {
        [(CALayer*)win->getLayer() release];
    }
#endif
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
#ifdef __LP64__
    // Neither Carbon nor QuickDraw exist for 64bit Mac OS X
    return NPERR_GENERIC_ERROR;
#else

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
            win = getFactoryInstance()->createPluginWindowCarbonQD(WindowContextQuickDraw((CGrafPtr)prt->port, prt->portx, prt->porty));
            win->setNpHost(m_npHost);
            pluginWin = static_cast<PluginWindow*>(win);
            win->setWindowPosition(window->x, window->y, window->width, window->height);
            win->setWindowClipping(window->clipRect.top,    window->clipRect.left,
                                   window->clipRect.bottom, window->clipRect.right);
            pluginMain->SetWindow(win);
        } else {
            win->setWindowPosition(window->x, window->y, window->width, window->height);
            win->setWindowClipping(window->clipRect.top,    window->clipRect.left,
                                   window->clipRect.bottom, window->clipRect.right);
        }        
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
#endif
}

NPError NpapiPluginMac::SetWindowCarbonCG(NPWindow* window) {
#if !FBMAC_USE_COREGRAPHICS || !FBMAC_USE_CARBON
    return NPERR_GENERIC_ERROR;
#else
    // SetWindow provides us with the window that our plugin should draw to.
    // In the Cocoa event model the window.window is null in the passed NPWindow

    PluginWindowMacCarbonCG* pluginWinCG = static_cast<PluginWindowMacCarbonCG*>(pluginWin);

    if (window != NULL) {
        if (pluginWin != NULL) {
            if (pluginWinCG->getNPCGContext() != (NP_CGContext*)window->window) {
                pluginMain->ClearWindow(); // Received new window, kill the old one
                delete pluginWin;
                pluginWin = NULL;
            } 
        }

        if (pluginWin == NULL) {
            // We have no plugin window associated with this plugin object.
            // Make a new plugin window object for FireBreath & our plugin.
            pluginWinCG = getFactoryInstance()->createPluginWindowCarbonCG(WindowContextCoreGraphics((NP_CGContext*)window->window));
            pluginWinCG->setNpHost(m_npHost);
            pluginWin = static_cast<PluginWindow*>(pluginWinCG);
            // Initialize the window position & clipping from the newly arrived NPWindow window
            pluginWinCG->setWindowPosition(window->x, window->y, window->width, window->height);
            pluginWinCG->setWindowClipping(window->clipRect.top, window->clipRect.left, 
                                        window->clipRect.bottom, window->clipRect.right);
            pluginMain->SetWindow(pluginWin);
        } else {
            // Initialize the window position & clipping from the newly arrived NPWindow window
            pluginWinCG->setWindowPosition(window->x, window->y, window->width, window->height);
            pluginWinCG->setWindowClipping(window->clipRect.top, window->clipRect.left, 
                                        window->clipRect.bottom, window->clipRect.right);
        }
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
#if !FBMAC_USE_COREGRAPHICS || !FBMAC_USE_COCOA
    return NPERR_GENERIC_ERROR;
#else
    PluginWindowMacCocoaCG* pluginWinCG = static_cast<PluginWindowMacCocoaCG*>(pluginWin);

    if (window != NULL) {
        if (pluginWin != NULL) {
            pluginMain->ClearWindow(); // Kill old window
            delete pluginWin;
            pluginWin = NULL;
        }

        if (pluginWin == NULL) {
            // No window associated with this plugin object.
            // Make a new window and associate with the object.
            pluginWinCG = getFactoryInstance()->createPluginWindowCocoaCG();
            pluginWinCG->setNpHost(m_npHost);
            pluginWin = static_cast<PluginWindow*>(pluginWinCG);
            // Initialize window position & clipping 
            pluginWinCG->setWindowPosition(window->x, window->y, window->width, window->height);
            pluginWinCG->setWindowClipping(window->clipRect.top, window->clipRect.left, 
                                    window->clipRect.bottom, window->clipRect.right);
            pluginMain->SetWindow(pluginWin);
        } else {
            // Initialize window position & clipping 
            pluginWinCG->setWindowPosition(window->x, window->y, window->width, window->height);
            pluginWinCG->setWindowClipping(window->clipRect.top, window->clipRect.left, 
                                        window->clipRect.bottom, window->clipRect.right);
        }
    } else if (pluginWin != NULL) {
        // Our window is gone, we should stop using it
        pluginMain->ClearWindow();
        delete pluginWin;
        pluginWin = NULL;
    }

    return NPERR_NO_ERROR;
#endif
}

NPError NpapiPluginMac::SetWindowCocoaCA(NPWindow* window) {
#if !FBMAC_USE_COREANIMATION || !FBMAC_USE_COCOA
    return NPERR_GENERIC_ERROR;
#else
    PluginWindowMacCocoaCA* pluginWinCA = static_cast<PluginWindowMacCocoaCA*>(pluginWin);
    
    if (window != NULL) {
        if (pluginWin == NULL) {
            // No window associated with this plugin object.
            // Make a new window and associate with the object.
            pluginWinCA = getFactoryInstance()->createPluginWindowCocoaCA();
            pluginWinCA->setNpHost(m_npHost);
            pluginWin = static_cast<PluginWindow*>(pluginWinCA);
            // Initialize window position & clipping 
            pluginWinCA->setWindowPosition(window->x, window->y, window->width, window->height);
            pluginWinCA->setWindowClipping(window->clipRect.top, window->clipRect.left, 
                                        window->clipRect.bottom, window->clipRect.right);
            pluginMain->SetWindow(pluginWin);
        } else {
            // Update window position & clipping 
            pluginWinCA->setWindowPosition(window->x, window->y, window->width, window->height);
            pluginWinCA->setWindowClipping(window->clipRect.top, window->clipRect.left, 
                                        window->clipRect.bottom, window->clipRect.right);
        }
    } else if (pluginWin != NULL) {
        // Our window is gone, we should stop using it
        pluginMain->ClearWindow();
        delete pluginWin;
        pluginWin = NULL;
    }
    
    return NPERR_NO_ERROR;
#endif    
}

NPError NpapiPluginMac::SetWindowCocoaICA(NPWindow* window) {
#if !FBMAC_USE_COREANIMATION || !FBMAC_USE_COCOA
    return NPERR_GENERIC_ERROR;
#else
    PluginWindowMacCocoaICA* pluginWinICA = static_cast<PluginWindowMacCocoaICA*>(pluginWin);
    
    if (window != NULL) {
        if (pluginWin == NULL) {
            // No window associated with this plugin object.
            // Make a new window and associate with the object.
            pluginWinICA = getFactoryInstance()->createPluginWindowCocoaICA();
            pluginWinICA->setNpHost(m_npHost);
            pluginWin = static_cast<PluginWindow*>(pluginWinICA);
            // Initialize window position & clipping 
            pluginWinICA->setWindowPosition(window->x, window->y, window->width, window->height);
            pluginWinICA->setWindowClipping(window->clipRect.top, window->clipRect.left, 
                                        window->clipRect.bottom, window->clipRect.right);
            pluginMain->SetWindow(pluginWin);
        } else {
            // Update window position & clipping 
            pluginWinICA->setWindowPosition(window->x, window->y, window->width, window->height);
            pluginWinICA->setWindowClipping(window->clipRect.top, window->clipRect.left, 
                                        window->clipRect.bottom, window->clipRect.right);
        }
    } else if (pluginWin != NULL) {
        // Our window is gone, we should stop using it
        pluginMain->ClearWindow();
        delete pluginWin;
        pluginWin = NULL;
    }
#endif    
    return NPERR_NO_ERROR;
}

NPError NpapiPluginMac::SetWindow(NPWindow* window) {
    // Forward the SetWindow call to the appropriate SetWindow*() function
    switch(m_drawingModel) {
        case DrawingModelQuickDraw:
            return SetWindowCarbonQD(window);
        case DrawingModelCoreGraphics: {
            switch(m_eventModel) {
                case EventModelCarbon: {
                    return SetWindowCarbonCG(window);
                }
                case EventModelCocoa: {
                    return SetWindowCocoaCG(window);
                }
            }
        }
        case DrawingModelCoreAnimation: {
            return SetWindowCocoaCA(window);
        }
        case DrawingModelInvalidatingCoreAnimation:{
            return SetWindowCocoaICA(window);
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
    
    PluginWindowMacCarbon* win = dynamic_cast<PluginWindowMacCarbon*>(pluginWin);
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

int16_t NpapiPluginMac::GetValue(NPPVariable variable, void *value) {
#if FBMAC_USE_COREANIMATION
    if(variable == NPPVpluginCoreAnimationLayer) {
        if(m_drawingModel == DrawingModelCoreAnimation || m_drawingModel == DrawingModelInvalidatingCoreAnimation) {
            PluginWindowMacCocoaCA* win = static_cast<PluginWindowMacCocoaCA*>(pluginWin);
            if(win != NULL) {
                *((CALayer **)value) = (CALayer*)win->getLayer();
                [(CALayer *)win->getLayer() retain];
                return NPERR_NO_ERROR;
            }
            return NPERR_GENERIC_ERROR;
        } else {
            return NPERR_GENERIC_ERROR;
        }
    } 
#endif
    int16_t res = NpapiPlugin::GetValue(variable, value);
    if (res == NPERR_NO_ERROR && variable == NPPVpluginScriptableNPObject) {
        // Ready state means that we are ready to interact with Javascript;
        setReady();
    }
    return res;
}

void NpapiPluginMac::HandleCocoaTimerEvent() {
#if !FBMAC_USE_COCOA
    return;
#else
    PluginWindowMacCocoa* win = static_cast<PluginWindowMacCocoa*>(pluginWin);
    return win->handleTimerEvent();
#endif
}
