/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 15, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_NPAPITYPES
#define H_NPAPITYPES

#include "npupp.h"
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