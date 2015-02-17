/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Dec 3, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the FireBreath development team
\**********************************************************/

#include <boost/algorithm/string.hpp>
#include <memory>
#include "win_common.h"
#include "NpapiTypes.h"
#include "PluginCore.h"
#include "FactoryBase.h"
#include "Win/NpapiPluginWin.h"
#include "Win/PluginWindowWin.h"
#include "Win/PluginWindowlessWin.h"
#include "NpapiPluginFactory.h"
#include "PluginInfo.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#ifdef FBWIN_ASYNCSURFACE

#include "Win/NpapiAsyncDrawService.h"

struct DrawingModel {
    const char* name;
    NPNVariable query;
    NPDrawingModel model;

    bool negotiate(const FB::Npapi::NpapiBrowserHostPtr& host, std::string requested, boost::function<void(NPDrawingModel)> factory) const
    {
        using namespace boost::algorithm;
        std::vector<std::string> prefs;
        split(prefs, requested, !is_alnum());

        bool gotone = false;
        for (size_t i = 0; !gotone && i < prefs.size(); i++) {
            const DrawingModel* dm = this;
            while (dm->name && strcmp(prefs[i].c_str(), dm->name)) dm++;

            NPBool supported = false;
            if (dm->name &&
                NPERR_NO_ERROR == host->GetValue(dm->query, &supported) && supported &&
                NPERR_NO_ERROR == host->SetValue(NPPVpluginDrawingModel, (void*) dm->model))
            {
                factory(dm->model);
                gotone = true;
            }
        }
        return gotone;
    }

};

#define FB_DRAWING_MODEL(x) { #x, NPNVsupports##x##Bool, NPDrawingModel##x }
#define FB_DRAWING_MODEL_FBLEGACYNAME(x, f) { "NPDrawingModel"#x, NPNVsupports##x##Bool, NPDrawingModel##x }
#define FB_DRAWING_MODEL_END_LIST { 0, (NPNVariable) 0, (NPDrawingModel) 0 }

static const DrawingModel g_supportedModels[] = {
    // FB_DRAWING_MODEL(AsyncBitmapSurface), // still to do
    FB_DRAWING_MODEL(AsyncWindowsDXGISurface),
    FB_DRAWING_MODEL_END_LIST
};

void FB::Npapi::NpapiPluginWin::pluginWindowFactory(NPDrawingModel model)
{
    // todo: support AsyncBitmapSurface as well
    AsyncDrawServicePtr asd = std::make_shared<NpapiAsyncDrawService>(m_npHost);
    PluginWindow* pw = getFactoryInstance()->createPluginWindowless(WindowContextWindowless(NULL, asd));
    pluginWin.swap(std::unique_ptr<PluginWindow>(pw));
    m_drawingModel = model;
}

#endif

using namespace FB::Npapi;

extern std::string g_dllPath;

FB::Npapi::NpapiPluginPtr FB::Npapi::createNpapiPlugin(const FB::Npapi::NpapiBrowserHostPtr& host, std::string mimetype)
{
    return std::make_shared<NpapiPluginWin>(host, mimetype);
}

NpapiPluginWin::NpapiPluginWin(const NpapiBrowserHostPtr& host, std::string mimetype)
    : NpapiPlugin(host, mimetype)
    , m_drawingModel((NPDrawingModel)-1)
{
    PluginCore::setPlatform("Windows", "NPAPI");
    setFSPath(g_dllPath);
}

NpapiPluginWin::~NpapiPluginWin()
{
    pluginMain->ClearWindow();
}

void NpapiPluginWin::init(NPMIMEType pluginType, int16_t argc, char* argn[], char *argv[])
{
    NpapiPlugin::init(pluginType, argc, argn, argv);

    if (!FB::pluginGuiEnabled() || pluginMain->isWindowless()) {
        /* Windowless plugins require negotiation with the browser.
        * If the plugin does not set this value it is assumed to be
        * a windowed plugin.
        * See: https://developer.mozilla.org/en/Gecko_Plugin_API_Reference/Drawing_and_Event_Handling
        */
        m_npHost->SetValue(NPPVpluginWindowBool, (void*)false);
        m_npHost->SetValue(NPPVpluginTransparentBool, (void*)true); // Set transparency to true
    }

#ifdef FBWIN_ASYNCSURFACE
    bool negotiated = g_supportedModels->negotiate(
        m_npHost, 
        pluginMain->negotiateDrawingModel(),
        std::bind(&NpapiPluginWin::pluginWindowFactory, this, std::placeholders::_1));
#endif
    if (pluginWin) 
    {
        pluginMain->SetWindow(pluginWin.get());
    } 
}

void NpapiPluginWin::invalidateWindow( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom )
{
    NPRect r = { top, left, bottom, right };
    if (!m_npHost->isMainThread()) {
        m_npHost->ScheduleOnMainThread(m_npHost, std::bind(&NpapiBrowserHost::InvalidateRect2, m_npHost, r));
    } else {
        m_npHost->InvalidateRect(&r);
    }
}

NPError NpapiPluginWin::SetWindow(NPWindow* window)
{
    // If window == NULL then our window is gone. Stop drawing.
    // If window->window == NULL and we're a windowed plugin then the window is gone; otherwise not
    if(window == NULL || (window->window == NULL && window->type != NPWindowTypeDrawable)) {
        // Our window is gone
        if(pluginMain != NULL) {
            // Destroy our FireBreath window
            pluginMain->ClearWindow();
            pluginWin.reset();
        }
        return NPERR_NO_ERROR;
    }
    if (!pluginGuiEnabled())
        return NPERR_NO_ERROR;

    if (m_drawingModel == NPDrawingModelAsyncWindowsDXGISurface) {
        PluginWindowlessWin* win = dynamic_cast<PluginWindowlessWin*>(pluginWin.get());
        assert(win);
        if (win) {
            win->setWindowPosition(window->x, window->y, window->width, window->height);
        }
    } else
    // Code here diverges depending on if 
    // the plugin is windowed or windowless.
    if(pluginMain->isWindowless()) { 
        assert(window->type == NPWindowTypeDrawable);
        PluginWindowlessWin* win = dynamic_cast<PluginWindowlessWin*>(pluginWin.get());

        if(win == NULL && pluginWin != NULL) {
            // We've received a window of a different type than the 
            // window we have been using up until now.
            // This is unlikely/impossible, but it's worth checking for.
            pluginMain->ClearWindow();
            pluginWin.reset();
        }

        if(pluginWin == NULL) {
            // Create new window
            win = getFactoryInstance()->createPluginWindowless(FB::WindowContextWindowless(NULL));
            HWND browserHWND;
            m_npHost->GetValue(NPNVnetscapeWindow, (void*)&browserHWND); 
            win->setHWND(browserHWND);
            win->setWindowPosition(window->x, window->y, window->width, window->height);
            win->setWindowClipping(window->clipRect.top, window->clipRect.left,
                                   window->clipRect.bottom, window->clipRect.right);
            win->setInvalidateWindowFunc(std::bind(&NpapiPluginWin::invalidateWindow, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            pluginMain->SetWindow(win);
            pluginWin.reset(win);
        } else if (win) {
            win->setWindowPosition(window->x, window->y, window->width, window->height);
            win->setWindowClipping(window->clipRect.top, window->clipRect.left,
                                   window->clipRect.bottom, window->clipRect.right);
        }
    } else { 
        assert(window->type == NPWindowTypeWindow);
        PluginWindowWin* win = dynamic_cast<PluginWindowWin*>(pluginWin.get());
        // Check to see if we've received a new HWND (new window)
        if(win != NULL && win->getHWND() != (HWND)window->window) {
            pluginMain->ClearWindow();
            pluginWin.reset() ;
        } else if(win == NULL && pluginWin != NULL) {
            // We've received a window of a different type than the 
            // window we have been using up until now.
            // This is unlikely/impossible, but it's worth checking for.
            pluginMain->ClearWindow();
            pluginWin.reset();
        }
    
        if(pluginWin == NULL) {
            // Sanity check
            assert(::IsWindow((HWND)window->window));
            // Create new window
            HWND browserHWND;
            m_npHost->GetValue(NPNVnetscapeWindow, (void*)&browserHWND); 
            win = getFactoryInstance()->createPluginWindowWin(FB::WindowContextWin((HWND)window->window));
            win->setBrowserHWND(browserHWND);
            pluginMain->SetWindow(win);
            pluginWin.reset(win);
        }    
    }

    return NPERR_NO_ERROR;
}

int16_t NpapiPluginWin::HandleEvent(void* event) {
    PluginWindowlessWin* win = dynamic_cast<PluginWindowlessWin*>(pluginWin.get());
    if(win != NULL) {
        LRESULT lRes(0);
        NPEvent* evt(reinterpret_cast<NPEvent*>(event));
        if (evt->event == WM_PAINT) {  //special handle drawing, as we need to pass the draw bounds through the layers
            FB::Rect bounds;
            if (evt->lParam) {  // some browsers pass through bounds in lParam, but Safari does not (as of 5.0.5)
                RECT *prc = (RECT*) evt->lParam;  // not 64-bit safe, but it's how NPAPI works
                bounds.top = prc->top;
                bounds.left = prc->left;
                bounds.bottom = prc->bottom;
                bounds.right = prc->right;
            } else {
                bounds = win->getWindowPosition();
            }
            return win->HandleDraw((HDC)evt->wParam, bounds);
        } else if (win->HandleEvent(evt->event, static_cast<uint32_t>(evt->wParam), evt->lParam, lRes)) {
            return boost::numeric_cast<int16_t>(lRes);
        }
    }
    return false;
}

