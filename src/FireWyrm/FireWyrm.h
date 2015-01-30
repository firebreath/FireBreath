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

typedef uint8_t FW_RESULT;
typedef uint32_t FW_INST;
typedef uint32_t FW_MSG;
typedef RESULT (*FW_Callback)(const FW_INST hndInst, FW_MSG hndMsg, const uint8_t* strResp, uint32_t strRespLen);

#endif // H_FIREWYRM




