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
        BrowserHostWrapper() : refCount(0) { }
        virtual ~BrowserHostWrapper() { }

    public:
        virtual bool FireMethod(FB::EventHandlerObject *target,
            std::vector<FB::variant>& args) = 0;
        virtual bool FireMethod(std::string name, FB::EventHandlerObject *target,
            std::vector<FB::variant>& args) = 0;
        virtual void *getContextID() = 0;

    protected:
        unsigned int refCount;
    public:
        void AddRef() { refCount++; }
        unsigned int Release()
        {
            if (--refCount == 0) {
                delete this;
                return 0;
            }
            return refCount;
        }
    };
}

#endif