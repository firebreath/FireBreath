/**********************************************************\
Original Author: Matthias (nitrogenycs)

Created:    Mar 7, 2010
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass Inc, Georg Fritzsche,
               Firebreath development team
\**********************************************************/

namespace FB
{
    class BrowserHostWrapper;
}

class StreamsTest
{
public:
    StreamsTest( FB::BrowserHostWrapper* host );

    bool run();

protected:
    FB::BrowserHostWrapper* host;
};