/**********************************************************\

  Auto-generated FBTestPlugin.cpp

  This file contains the auto-generated main plugin object
  implementation for the ${PLUGIN.name} project

\**********************************************************/

#define _USE_MATH_DEFINES

#include <math.h>
#include <sstream>
#include <chrono>
#include <stdio.h>
#include <functional>

#include "FBTestPluginAPI.h"
#include "SimpleMathAPI.h"
#include "FBTestPlugin.h"
#include "DOM/Window.h"
#include "URI.h"

#ifdef FB_WIN
#include "PluginWindowWin.h"
#include "PluginWindowlessWin.h"
#ifdef FBWIN_ASYNCSURFACE
#include "Win/D3d10AsyncDrawService.h"
#endif
#endif

void FBTestPlugin::StaticInitialize()
{
    FBLOG_INFO("StaticInit", "Static Initialize");
    // Place one-time initialization stuff here; As of FireBreath 1.4 this should only
    // be called once per process
}

void FBTestPlugin::StaticDeinitialize()
{
    FBLOG_INFO("StaticInit", "Static Deinitialize");
    // Place one-time deinitialization stuff here. This should be called just before
    // the plugin library is unloaded
}

FBTestPlugin::FBTestPlugin(std::string mimetype) :
    m_mimetype(mimetype)
#ifdef FBWIN_ASYNCSURFACE
    , m_threadInterrupted(true)
#endif
{
}

FBTestPlugin::~FBTestPlugin()
{
    // By resetting the api and then telling the host that the plugin instance is shutting down,
    // we control the lifecycle. As long as m_api isn't stored anywhere else, telling host to
    // shutdown should free the object
    releaseRootJSAPI();
    m_host->freeRetainedObjects();
}

FB::JSAPIPtr FBTestPlugin::createJSAPI()
{
    // Return a SimpleMath object instead of a FBTestPluginAPI:
    if (!m_mimetype.compare("application/x-fbtestplugin-math")) {
        // Create a new simplemath object each time        
        return std::make_shared<SimpleMathAPI>(m_host);
    }

    // By default, return a FBTestPluginAPI object:
    // m_host is the BrowserHost
    return std::make_shared<FBTestPluginAPI>(std::dynamic_pointer_cast<FBTestPlugin>(shared_from_this()), m_host);
}

bool FBTestPlugin::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow*)
{
    // These are just examples; if you don't need them, remove them, as htmlLog
    // does have a performance penalty!
    std::ostringstream oStr;
    oStr << "Mouse down at: " << evt->m_x << ", " << evt->m_y;
    this->m_host->htmlLog(oStr.str());
    return false;
}

bool FBTestPlugin::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow*)
{
    // These are just examples; if you don't need them, remove them, as htmlLog
    // does have a performance penalty!
    std::ostringstream oStr;
    oStr << "Mouse up at: " << evt->m_x << ", " << evt->m_y;
    this->m_host->htmlLog(oStr.str());
    return false;
}

bool FBTestPlugin::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow*)
{
    // These are just examples; if you don't need them, remove them, as htmlLog
    // does have a performance penalty!
    std::ostringstream oStr;
    oStr << "Mouse move at: " << evt->m_x << ", " << evt->m_y;
    this->m_host->htmlLog(oStr.str());
    return false;
}

bool FBTestPlugin::onAttached( FB::AttachedEvent *evt, FB::PluginWindow* win)
{
#ifdef FBWIN_ASYNCSURFACE
    // This is called when the window is attached; don't start drawing before this!
    FB::PluginWindowlessWin* windowless = dynamic_cast<FB::PluginWindowlessWin*>(win);
    if (windowless) {
        FB::D3d10AsyncDrawServicePtr ads = std::dynamic_pointer_cast<FB::D3d10AsyncDrawService>(windowless->getAsyncDrawService());
        if (ads) {
            startDrawAsync(ads);
        }
    }
#endif
    return false;
}

bool FBTestPlugin::onDetached( FB::DetachedEvent *evt, FB::PluginWindow* )
{
    // This is called when the window is detached! You must not draw after this!
    return false;
}

bool FBTestPlugin::draw( FB::RefreshEvent *evt, FB::PluginWindow* win )
{
    FB::Rect pos = win->getWindowPosition();

#if FB_WIN
    HDC hDC;
    FB::PluginWindowlessWin *wndLess = dynamic_cast<FB::PluginWindowlessWin*>(win);
    FB::PluginWindowWin *wnd = dynamic_cast<FB::PluginWindowWin*>(win);
    PAINTSTRUCT ps;
    if (wndLess) {
#ifdef FBWIN_ASYNCSURFACE
        if (wndLess->getAsyncDrawService()) {
            // Firefox still calls draw when we are async drawing! (IE doesn't)
            return true;
        }
#endif
        hDC = wndLess->getHDC();
    } else if (wnd) {
        hDC = BeginPaint(wnd->getHWND(), &ps);
        pos.right -= pos.left;
        pos.left = 0;
        pos.bottom -= pos.top;
        pos.top = 0;
    } else {
        return true;
    }

    ::SetTextAlign(hDC, TA_CENTER|TA_BASELINE);
    LPCTSTR pszText = _T("FireBreath Plugin!");
    if (!m_mimetype.compare("application/x-fbtestplugin-math")) {
        // Mark 2nd mimetype differently than main mimetype
        pszText =  _T("FireBreath Plugin (Math)!");
    }
    ::TextOut(hDC, pos.left + (pos.right - pos.left) / 2, pos.top + (pos.bottom - pos.top) / 2, pszText, lstrlen(pszText));

    if (wnd) {
        // Release the device context
        EndPaint(wnd->getHWND(), &ps);
    }
#endif

    return true;
}

#ifdef FBWIN_ASYNCSURFACE

bool FBTestPlugin::startDrawAsync(FB::D3d10AsyncDrawServicePtr ads)
{
    m_thread = std::thread(&FBTestPlugin::renderThread, this, ads);
    return true;
}

struct Scene
{
    uint32_t _frame;
    uint32_t _abgr; 

    Scene(uint32_t abgrBackground)
        : _frame(0)
        , _abgr(abgrBackground)
    {}

    bool render(ID3D10Device1* device, ID3D10RenderTargetView* rtView, uint32_t width, uint32_t height)
    {
        uint32_t r = (_abgr & 0xFF);
        uint32_t g = (_abgr & 0xFF00) >> 8;
        uint32_t b = (_abgr & 0xFF0000) >> 16;
        float a = (float) (sin(_frame / 31.0 * M_PI) / 2 + 0.5);
        float color[4] = { b * a / 255.f, g * a / 255.f, r * a / 255.f, a };
        device->ClearRenderTargetView(rtView, color);
        // insert fancy demo here
        ++_frame;
        return true;
    }
};

void FBTestPlugin::renderThread(FB::D3d10AsyncDrawServicePtr ads)
{
    Scene scene(asyncTestBgColor());
    do {
        ads->render(std::bind(&Scene::render, &scene, _1, _2, _3, _4));
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    } while(!m_threadInterrupted);
}

//virtual
void FBTestPlugin::ClearWindow()
{
    // the window is about to go away, so we need to stop our render thread
    m_threadInterrupted = true;
    m_thread.join();
}

#endif

void FBTestPlugin::onPluginReady()
{
    // TODO: make this work
    //FB::DOM::WindowPtr window = m_host->getDOMWindow();
    //if (!window)
    //    return;

    //FB::BrowserHostWeakPtr weakHost(m_host);
    //window->getLocation().done([weakHost](std::string loc) {
    //    FB::URI uri = FB::URI::fromString(loc);
    //    FB::BrowserHostPtr host(weakHost.lock());
    //    if (!host) return;
    //    if (uri.query_data.find("log") != uri.query_data.end()) {
    //        host->setEnableHtmlLog(true);
    //    } else {
    //        host->setEnableHtmlLog(false);
    //    }
    //});
}

uint32_t FBTestPlugin::asyncTestBgColor()
{
    if (m_asyncTestBgColor) {
        return *m_asyncTestBgColor;
    }

    FB::VariantMap::iterator itr = m_params.find("color");
    if (itr != m_params.end()) {
        try {
            uint32_t v;
            std::stringstream ss;
            ss << std::hex << itr->second.convert_cast<std::string>();
            ss >> v;
            m_asyncTestBgColor = v;
            return *m_asyncTestBgColor;
        } catch (const FB::bad_variant_cast& ex) {
            FB_UNUSED_VARIABLE(ex);
        }
    }
    m_asyncTestBgColor = 0x7F00FF00; // if no param is given set the default to semi-transparent green (format is 0xAARRGGBB)
    return *m_asyncTestBgColor;
}

std::string FBTestPlugin::negotiateDrawingModel()
{
    boost::optional<std::string> param = getParam("drawingmodel");
    return param ? *param : "";
}
