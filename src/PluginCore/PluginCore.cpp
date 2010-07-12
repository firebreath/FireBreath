/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#include "PluginWindow.h"
#include "JSAPI.h"
#include "FactoryDefinitions.h"
#include "BrowserHostWrapper.h"

#include "PluginCore.h"

using namespace FB;

/***************************\
 Static initialization stuff
\***************************/

volatile int PluginCore::ActivePluginCount = 0;

std::string PluginCore::OS;
std::string PluginCore::Browser;
void PluginCore::setPlatform(std::string os, std::string browser)
{
    PluginCore::OS = os;
    PluginCore::Browser = browser;
}

/***************************\
     Regular Class Stuff
\***************************/

PluginCore::PluginCore() : m_Window(NULL), m_paramsSet(false)
{
    // This class is only created on the main UI thread,
    // so there is no need for mutexes here
    if (++PluginCore::ActivePluginCount == 1) {
        // Only on the first initialization
        GlobalPluginInitialize();
    }

    
}

PluginCore::PluginCore(std::set<std::string> params)
    : m_Window(NULL), m_supportedParamSet(params), m_paramsSet(false)
{
    // This class is only created on the main UI thread,
    // so there is no need for mutexes here
    if (++PluginCore::ActivePluginCount == 1) {
        // Only on the first initialization
        GlobalPluginInitialize();
    }
    m_supportedParamSet.insert("onload");
    m_supportedParamSet.insert("onerror");
    m_supportedParamSet.insert("onlog");
}

PluginCore::~PluginCore()
{
    // This class is only destroyed on the main UI thread,
    // so there is no need for mutexes here
    if (--PluginCore::ActivePluginCount == 0) {
        // Only on the destruction of the final plugin instance
        GlobalPluginDeinitialize();
    }
}

StringSet* PluginCore::getSupportedParams()
{
    return &m_supportedParamSet;
}

void PluginCore::setParams(const FB::VariantMap& inParams)
{
    m_params.insert(inParams.begin(), inParams.end());
}

void PluginCore::SetHost(BrowserHostWrapper *host)
{
    m_host = host;
}

JSAPI* PluginCore::getRootJSAPI()
{
    if (m_api.ptr() == NULL) {
        m_api = createJSAPI();
    }

    return m_api.ptr();
}

PluginWindow* PluginCore::GetWindow() const
{
    return m_Window;
}

void PluginCore::SetWindow(PluginWindow *wind)
{
    if (m_Window && m_Window != wind) {
        ClearWindow();
    }
    m_Window = wind;
    wind->AttachObserver(this);
}

void PluginCore::ClearWindow()
{
    if (m_Window) {
        m_Window->DetachObserver(this);
        m_Window = NULL;
    }
}