/**********************************************************\
Original Author: Matthias (nitrogenycs)

Created:    Mar 7, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass Inc, Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#include "BrowserHost.h"

class StreamsTest
{
public:
    StreamsTest( FB::BrowserHostPtr host );

    bool run();

protected:
    FB::BrowserHostPtr host;
};

