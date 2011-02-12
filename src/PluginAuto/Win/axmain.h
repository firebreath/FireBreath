/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 17, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_AXMAIN
#define H_AXMAIN

#include "FireBreathWin_i.h"
#include "global/resource.h"
#include <atlbase.h>

class CFireBreathWinModule : public ATL::CAtlDllModuleT< CFireBreathWinModule >
{
public :
    CFireBreathWinModule();
    DECLARE_LIBID(FB_LIBID)
    DECLARE_REGISTRY_APPID_RESOURCEID(IDR_FIREBREATHWIN, "{B415CD14-B45D-4BCA-B552-B06175C38606}")
};

extern class CFireBreathWinModule _AtlModule;

#endif H_AXMAIN

