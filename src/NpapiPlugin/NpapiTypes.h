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

#ifndef H_NPAPITYPES
#define H_NPAPITYPES

#ifdef _WINDOWS
#include "Win/win_common.h"
#endif

#include "nptypes.h"
#include "npfunctions.h"
#include "npapi.h"
#include "npruntime.h"
#include "variant.h"

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

    void copyNPBrowserFuncs(NPNetscapeFuncs *dstFuncs, NPNetscapeFuncs *srcFuncs);

    typedef FB::variant_detail::empty NpapiNull;

}; };

#endif