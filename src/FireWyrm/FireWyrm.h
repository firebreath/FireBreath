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
\*************************************************&&&&****************/

#pragma once
#ifndef H_FIREWYRM
#define H_FIREWYRM

#include <cstdint>

typedef uint8_t  FW_RESULT;
typedef uint32_t FW_INST;
typedef uint32_t FW_MSG;


typedef FW_RESULT (*FW_AsyncCall)(const void* pData);

///
// FireWyrmHost calls -- calls made back into whatever is hosting the plugin, such as the browser
///
typedef FW_RESULT (*FWH_PerformAsyncCall)(const FW_AsyncCall* call, const void* pData);

typedef FW_RESULT (*FW_Command)(const FW_INST colonyId, const char* strCommand, uint32_t strCommandLen);
typedef FW_RESULT (*FW_CommandCallback)(const FW_INST colonyId, const uint8_t* strResp, uint32_t strRespLen);

typedef struct _FireWyrmHostFuncs
{
    uint16_t size;
    uint16_t version;
    FW_Command call;
    FW_CommandCallback cmdCallback;
    FWH_PerformAsyncCall doAsyncCall;
} FWHostFuncs;

typedef struct _FireWyrmColonyFuncs
{
    uint16_t size;
    uint16_t version;
    FW_Command call;
    FW_CommandCallback cmdCallback;
} FWColonyFuncs;



#endif // H_FIREWYRM


