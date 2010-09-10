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

#include "BrowserHostWrapper.h"

class StreamsTest
{
public:
    StreamsTest( FB::BrowserHost host );

    bool run();

protected:
    FB::BrowserHost host;
};
