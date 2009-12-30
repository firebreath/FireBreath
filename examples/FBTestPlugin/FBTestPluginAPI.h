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

class FBTestPluginAPI : public FB::JSAPIAuto
{
public:
    FBTestPluginAPI(FB::BrowserHost host);
    virtual ~FBTestPluginAPI();

    // Read/Write property testString
    std::string get_testString();
    void set_testString(std::string val);

    // Read-only property someInt
    long get_someInt();

    FB::JSOutObject get_simpleMath();
    FB::variant echo(FB::variant a);

    std::string asString(FB::variant a);
    bool asBool(FB::variant a);
    long asInt(FB::variant a);
    double asDouble(FB::variant a);

    std::string listArray(std::vector<std::string>);
    FB::JSOutArray reverseArray(std::vector<std::string> arr);
    FB::JSOutArray getObjectKeys(FB::JSObject arr);
    FB::JSOutArray getObjectValues(FB::JSObject arr);
    FB::JSOutMap getUserData();

    // Method add
    long add(long a, long b);

private:
    FB::BrowserHost m_host;
    FB::JSOutObject m_simpleMath;

    std::string m_testString;
};
