/**********************************************************\ 
Original Authors: Richard Bateman (taxilian)
                  Don Jordan (kc7zax)

Created:    Oct 15, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_NPAPIPLUGINMODULE
#define H_NPAPIPLUGINMODULE

#include "NpapiBrowserHost.h"

namespace FB
{
    namespace Npapi {
    
    class NpapiPluginModule
    {
    public:
        NpapiPluginModule();
        virtual ~NpapiPluginModule(void);

        void setNetscapeFuncs(NPNetscapeFuncs *npnFuncs);
    };

    };
};

#endif