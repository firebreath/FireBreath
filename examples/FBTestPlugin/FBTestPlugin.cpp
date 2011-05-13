/**********************************************************\

  Auto-generated FBTestPlugin.cpp

  This file contains the auto-generated main plugin object
  implementation for the ${PLUGIN.name} project

\**********************************************************/

#include <sstream>
#include "FBTestPluginAPI.h"
#include <stdio.h>

#include "FBTestPlugin.h"
#include "DOM/Window.h"
#include "URI.h"

#ifdef FB_WIN
#include "PluginWindowWin.h"
#include "PluginWindowlessWin.h"
#ifdef HAS_LEAKFINDER
#define XML_LEAK_FINDER
#include "LeakFinder/LeakFinder.h"
#endif
#endif


#ifdef HAS_LEAKFINDER
boost::scoped_ptr<LeakFinderXmlOutput> FBTestPlugin::pOut;
#endif

void FBTestPlugin::StaticInitialize()
{
    FBLOG_INFO("StaticInit", "Static Initialize");
    // Place one-time initialization stuff here; As of FireBreath 1.4 this should only
    // be called once per process

#ifdef HAS_LEAKFINDER
#ifdef XML_LEAK_FINDER
    pOut.swap(boost::scoped_ptr<LeakFinderXmlOutput>(new LeakFinderXmlOutput(L"C:\\code\\firebreath_mem.xml")));
#endif
    InitLeakFinder();
#endif
}

void FBTestPlugin::StaticDeinitialize()
{
#ifdef HAS_LEAKFINDER
    DeinitLeakFinder(pOut.get());
    pOut.reset();
#endif
    FBLOG_INFO("StaticInit", "Static Deinitialize");
    // Place one-time deinitialization stuff here. This should be called just before
    // the plugin library is unloaded

}


FBTestPlugin::FBTestPlugin()
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
    // m_host is the BrowserHost
    return boost::make_shared<FBTestPluginAPI>(FB::ptr_cast<FBTestPlugin>(shared_from_this()), m_host);
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

bool FBTestPlugin::onAttached( FB::AttachedEvent *evt, FB::PluginWindow* )
{
    // This is called when the window is attached; don't start drawing before this!
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
        hDC = wndLess->getHDC();
    } else if (wnd) {
        hDC = BeginPaint(wnd->getHWND(), &ps);
        pos.right -= pos.left;
        pos.left = 0;
        pos.bottom -= pos.top;
        pos.top = 0;
    }

    ::SetTextAlign(hDC, TA_CENTER|TA_BASELINE);
    LPCTSTR pszText = _T("FireBreath Plugin!");
    ::TextOut(hDC, pos.left + (pos.right - pos.left) / 2, pos.top + (pos.bottom - pos.top) / 2, pszText, lstrlen(pszText));

    if (wnd) {
        // Release the device context
        EndPaint(wnd->getHWND(), &ps);
    }
#endif
    return true;
}

bool FBTestPlugin::isWindowless()
{
    return PluginCore::isWindowless();
    //return true;
}

void FBTestPlugin::onPluginReady()
{
    FB::URI uri = FB::URI::fromString(m_host->getDOMWindow()->getLocation());
    if (uri.query_data.find("log") != uri.query_data.end()) {
        m_host->setEnableHtmlLog(true);
    } else {
        m_host->setEnableHtmlLog(false);
    }
}
