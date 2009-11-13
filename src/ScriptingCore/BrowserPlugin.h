/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_BROWSERPLUGIN
#define H_FB_BROWSERPLUGIN

#include "APITypes.h"
#include "AutoPtr.h"
#include "JSAPI.h"

namespace FB {

    // This class represents an instance of a browser plugin
    // (an individual object tag, for example)
    class BrowserPlugin
    {
    public:
        BrowserPlugin();
        virtual ~BrowserPlugin();

        virtual void shutdown() = 0;

    protected:
        AutoPtr<JSAPI> m_api;
    };

};
#endif
