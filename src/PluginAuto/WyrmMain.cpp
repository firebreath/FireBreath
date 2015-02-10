/*********************************************************************\
    Original Author: Richard Bateman

    Created:    Jan 18, 2015
    License:    Dual license model; choose one of two:
                New BSD License
                http://www.opensource.org/licenses/bsd-license.php
                - or -
                GNU Lesser General Public License, version 2.1
                http://www.gnu.org/licenses/lgpl-2.1.html

    Copyright 2015 Richard Bateman and the Firebreath development team
\*********************************************************************/

#if 0

#include "FireWyrm.h"
#include "WyrmPluginModule.h"
#include "logging.h"

extern "C" {
    FW_RESULT FireWyrm_Init(WyrmCallback* cb);
    FW_INST FireWyrm_New(const uint8_t* name, uint32_t nameLen);
    FW_MSG FireWyrm_Call(const FW_INST hndInst, const uint8_t* id, uint32_t idLen, const char* payload, uint32_t payloadLen);
    FW_RESULT FW_OSCALL FireWrym_Destroy(const FW_INST hndInst);
    FW_RESULT FireWyrm_Finit();
}

FW_RESULT FW_OSCALL FireWyrm_Init(WyrmCallback* cb) {
    FBLOG_INFO("FireWyrm", "Initializing FireWyrm plugin");
    WyrmPluginModule* mod = WyrmPluginModule::GetModule(0);
    mod->setCallback(cb);
}
FW_INST FW_OSCALL FireWyrm_New(const uint8_t* name, uint32_t nameLen) {
    std::string strName(name, nameLen);
    FBLOG_INFO("FireWyrm", "Instantiating new FireWyrm plugin instance");
    WyrmPluginModule* mod = WyrmPluginModule::GetModule(0);
    return mod->newPlugin(strName);
}
FW_MSG FW_OSCALL FireWyrm_Call(const FW_INST hndInst, const uint8_t* id, uint32_t idLen, const char* payload, uint32_t payloadLen) {
    std::string strId(id, idLen);
    std::string strPayload(payload, payloadLen);
    FBLOG_DEBUG("FireWyrm", "FireWyrm plugin call.  Instance: " << hndInst << ", id: " << strId);
    FBLOG_TRACE("FireWyrm", "FireWyrm plugin call payload: " << strPayload);

    WyrmPluginModule* mod = WyrmPluginModule::GetModule(0);
    return mod->postMessage(hndInst, stdId, strPayload);
}
FW_RESULT FW_OSCALL FW_OSCALL FireWrym_Destroy(const FW_INST hndInst) {
    FBLOG_INFO("FireWyrm", "Destroying FireWyrm plugin instance");
    WyrmPluginModule* mod = WyrmPluginModule::GetModule(0);
    return mod->destroyPlugin(hndInst);
}
FW_RESULT FW_OSCALL FireWyrm_Finit() {
    FBLOG_INFO("FireWyrm", "Finalizing FireWyrm plugin");
    return WyrmPluginModule::ReleaseModule(0);
}

#endif