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

#include <boost/make_shared.hpp>
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

using namespace FB::Npapi;

extern std::string g_dllPath;

FB::Npapi::NpapiPluginPtr FB::Npapi::createNpapiPlugin(const FB::Npapi::NpapiBrowserHostPtr& host, const std::string& mimetype)
{
    return boost::make_shared<NpapiPluginWin>(host, mimetype);
}

NpapiPluginWin::NpapiPluginWin(const NpapiBrowserHostPtr& host, const std::string& mimetype)
    : NpapiPlugin(host, mimetype), pluginWin(NULL)
{
    PluginCore::setPlatform("Windows", "NPAPI");
    setFSPath(g_dllPath);
}

NpapiPluginWin::~NpapiPluginWin()
{
    delete pluginWin; pluginWin = NULL;
}

void NpapiPluginWin::invalidateWindow( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom )
{
    NPRect r = { top, left, bottom, right };
    if (!m_npHost->isMainThread()) {
        m_npHost->ScheduleOnMainThread(m_npHost, boost::bind(&NpapiBrowserHost::InvalidateRect2, m_npHost, r));
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
            delete pluginWin; pluginWin = NULL;
        }
        return NPERR_NO_ERROR;
    }
    if (!pluginGuiEnabled())
        return NPERR_NO_ERROR;

    // Code here diverges depending on if 
    // the plugin is windowed or windowless.
    if(pluginMain->isWindowless()) { 
        assert(window->type == NPWindowTypeDrawable);
        PluginWindowlessWin* win = dynamic_cast<PluginWindowlessWin*>(pluginWin);

        if(win == NULL && pluginWin != NULL) {
            // We've received a window of a different type than the 
            // window we have been using up until now.
            // This is unlikely/impossible, but it's worth checking for.
            pluginMain->ClearWindow();
            delete pluginWin; pluginWin = NULL;
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
            win->setInvalidateWindowFunc(boost::bind(&NpapiPluginWin::invalidateWindow, this, _1, _2, _3, _4));
            pluginMain->SetWindow(win);
            pluginWin = win;
        } else {
            win->setWindowPosition(window->x, window->y, window->width, window->height);
            win->setWindowClipping(window->clipRect.top, window->clipRect.left,
                                   window->clipRect.bottom, window->clipRect.right);
        }
    } else { 
        assert(window->type == NPWindowTypeWindow);
        PluginWindowWin* win = dynamic_cast<PluginWindowWin*>(pluginWin);
        // Check to see if we've received a new HWND (new window)
        if(win != NULL && win->getHWND() != (HWND)window->window) {
            pluginMain->ClearWindow();
            delete pluginWin; pluginWin = NULL; 
        } else if(win == NULL && pluginWin != NULL) {
            // We've received a window of a different type than the 
            // window we have been using up until now.
            // This is unlikely/impossible, but it's worth checking for.
            pluginMain->ClearWindow();
            delete pluginWin; pluginWin = NULL; 
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
            pluginWin = win;
        }    
    }

    return NPERR_NO_ERROR;
}

int16_t NpapiPluginWin::HandleEvent(void* event) {
    PluginWindowlessWin* win = dynamic_cast<PluginWindowlessWin*>(pluginWin);
    NPEvent* evt(reinterpret_cast<NPEvent*>(event));
    if(win != NULL) {
        LRESULT lRes(0);
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

