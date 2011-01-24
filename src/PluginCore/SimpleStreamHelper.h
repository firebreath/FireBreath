#pragma once
#ifndef SimpleStreamHelper_h__
#define SimpleStreamHelper_h__

/**********************************************************\
Original Author: Richard Bateman

Created:    Jan 24, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2011 Richard Bateman, 
               Firebreath development team
\**********************************************************/

#include "DefaultBrowserStreamHandler.h"
#include "PluginEvents/StreamEvents.h"
#include "BrowserStream.h"

namespace FB {
    FB_FORWARD_PTR(BrowserHost);

    class SimpleStreamHelper : public FB::DefaultBrowserStreamHandler
    {
    public:
        SimpleStreamHelper( const BrowserHostPtr& Host );


        BrowserHostPtr host;
    };
};
#endif // SimpleStreamHelper_h__