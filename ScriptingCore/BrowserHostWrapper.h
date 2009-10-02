/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 24, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_BROWSERHOSTWRAPPER
#define H_FB_BROWSERHOSTWRAPPER

#include "APITypes.h"

namespace FB
{
    class BrowserHostWrapper
    {
    public:
        BrowserHostWrapper() { }
        virtual ~BrowserHostWrapper() { }

    public:
        virtual bool FireEvent(std::string name, variant &target, std::vector<FB::variant>& args) = 0;
    };
}

#endif