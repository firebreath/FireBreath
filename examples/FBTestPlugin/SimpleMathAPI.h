/**********************************************************\
Original Author: Richard Bateman and Georg Fritzsche

Created:    December 3, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass Inc, Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#include <string>
#include <sstream>
#include "JSAPIAuto.h"
#include "BrowserHostWrapper.h"

class SimpleMathAPI : public FB::JSAPIAuto
{
public:
    SimpleMathAPI(FB::BrowserHost host);
    virtual ~SimpleMathAPI();

    // Read/Write property testString
    std::string get_testString();
    void set_testString(std::string val);

    // Read-only property someInt
    int get_someInt();

    // Method add
    long add(long a, long b);
    long sub(long a, long b);
    long mult(long a, long b);
    long div(long a, long b);

private:
    FB::BrowserHost m_host;

    std::string m_testString;
};
