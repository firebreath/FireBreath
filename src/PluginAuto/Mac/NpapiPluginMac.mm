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

#include "FactoryBase.h"

#include "NpapiPluginFactory.h"
#include <boost/make_shared.hpp>
#include <string>

#include "Mac/NpapiPluginMac.h"

using namespace FB::Npapi;

FB::Npapi::NpapiPluginPtr FB::Npapi::createNpapiPlugin(const FB::Npapi::NpapiBrowserHostPtr& host, const std::string& mimetype)
{
    return boost::make_shared<NpapiPluginMac>(host, mimetype);
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
    : NpapiPlugin(host, mimetype), m_eventModel((NPEventModel)-1), m_drawingModel((NPDrawingModel)-1)
{
    // If you receive a BAD_ACCESS error here you probably have something
    // wrong in your FactoryMain.cpp in your plugin project's folder

    PluginCore::setPlatform("Mac", "NPAPI");

    // Get the path to the bundle
    static const std::string pluginPath = getPluginPath();      
    setFSPath(pluginPath);
}

NpapiPluginMac::~NpapiPluginMac()
{
}

void NpapiPluginMac::init(NPMIMEType pluginType, int16_t argc, char* argn[], char *argv[]) {
    NpapiPlugin::init(pluginType, argc, argn, argv);

    // These HAVE to be called here during the NPP_New call to set the drawing and event models.
    boost::optional<std::string> drawingModel = pluginMain->getParam("drawingmodel");
    if (drawingModel)
        m_drawingModel = PluginWindowMac::initPluginWindowMac(m_npHost, drawingModel->c_str());
    else
        m_drawingModel = PluginWindowMac::initPluginWindowMac(m_npHost);
    m_eventModel = PluginEventMac::initPluginEventMac(m_npHost, m_drawingModel);

    // We can create our event model handler now.
    pluginEvt = PluginEventMacPtr(PluginEventMac::createPluginEventMac(m_eventModel));

    // We can create our drawing model handler now. It will be made for the chosen drawing model.
    pluginWin = PluginWindowMacPtr(PluginWindowMac::createPluginWindowMac(m_drawingModel));
    if (pluginWin)
    {
        pluginWin->setNpHost(m_npHost);
        // Tell the event model which window to use for events.
        if (pluginEvt)
            pluginEvt->setPluginWindow(pluginWin);
        pluginWin->setPluginEvent(pluginEvt);

        boost::optional<std::string> showdrawingmodel = pluginMain->getParam("showdrawingmodel");
        if (showdrawingmodel && (0 != strcmp(showdrawingmodel->c_str(), "0")))
            pluginWin->setShowDrawingModel(true);
    }
}

NPError NpapiPluginMac::SetWindow(NPWindow* window) {
    NPError res = NPERR_NO_ERROR;
    if (window)
    {
        if (pluginWin)
        {
            // Set the Window.
            res = pluginWin->SetWindow(window);

            // Notify the PluginCore about our new window. This will post an AttachedEvent.
            if (!pluginMain->GetWindow())
                pluginMain->SetWindow(pluginWin.get());
        }
    } else {
        // Notify the PluginCore about our missing window. This will post an DetachedEvent.
        pluginMain->ClearWindow();
        pluginWin.reset();
    }
    return res;
}

int16_t NpapiPluginMac::HandleEvent(void* event)
{
    if (pluginEvt)
        return pluginEvt->HandleEvent(event);
    return 0;
}

int16_t NpapiPluginMac::GetValue(NPPVariable variable, void *value) {
    int16_t res = NPERR_GENERIC_ERROR;
    switch (variable) {
        case NPPVpluginCoreAnimationLayer:
        {
            if (pluginWin) {
                // The PluginWindow owns the CALayer.
                res = pluginWin->GetValue(variable, value);
            
                // Notify the PluginCore about our new window. This will post an AttachedEvent.
                if (!pluginMain->GetWindow())
                    pluginMain->SetWindow(pluginWin.get());
            }
            FBLOG_INFO("PluginCore", "GetValue(NPPVpluginCoreAnimationLayer)");
        }   break;
        default:
            res = NpapiPlugin::GetValue(variable, value);
            break;
    }
    return res;
}

void NpapiPluginMac::HandleTimerEvent() {
    if (pluginWin)
        return pluginWin->handleTimerEvent();
}
