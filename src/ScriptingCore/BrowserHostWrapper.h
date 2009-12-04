/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 24, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

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
        virtual void ScheduleAsyncCall(void (*func)(void *), void *userData) = 0;

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