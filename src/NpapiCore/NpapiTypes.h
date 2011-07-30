/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 15, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_NPAPITYPES
#define H_NPAPITYPES

#ifdef FB_WIN
#include "win_common.h"
#endif
#ifdef FB_MACOSX 
#include <Carbon/Carbon.h>
#endif

#include "nptypes.h"
#include "npfunctions.h"
#include "npapi.h"
#include "npruntime.h"
#include "variant.h"

#if !defined(__QUICKDRAWAPI__) && !defined(NP_NO_QUICKDRAW)
#define NP_NO_QUICKDRAW
#endif

namespace FB { namespace Npapi {

#ifdef LINUX
    typedef NPError (*NPInitFuncPtr)(NPNetscapeFuncs *aNPNFuncs, NPPluginFuncs *aNPPFuncs);
    typedef void (*NPShutdownFuncPtr)(void);
    typedef void* NPGetEntryPointsFuncPtr;
#else
    typedef NPError (*NPInitFuncPtr)(NPNetscapeFuncs *aNPNFuncs);
    typedef void (*NPShutdownFuncPtr)(void);
    typedef NPError (*NPGetEntryPointsFuncPtr)(NPPluginFuncs* pFuncs);
#endif

    void copyNPBrowserFuncs(NPNetscapeFuncs *dstFuncs, NPNetscapeFuncs *srcFuncs, NPP instance = NULL);

    typedef FB::FBNull NpapiNull;
    typedef FB::FBVoid NpapiVoid;

}; };

#endif

