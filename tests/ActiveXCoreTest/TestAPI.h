/**********************************************************\ 
Original Author: Richard Bateman and Georg Fritzsche 

Created:    Aug 9, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass Inc, Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#include <string>
#include <sstream>
#include "JSAPIAuto.h"
#include "BrowserHost.h"

class TestAPI : public FB::JSAPIAuto
{
public:
    TestAPI(FB::BrowserHostPtr host);
    virtual ~TestAPI();

    // Read/Write property testString
    std::string get_testString();
    void set_testString(const std::string& val);

    // Read-only property someInt
    long get_someInt();

    FB::variant echo(const FB::variant& a);

    std::string asString(const FB::variant& a);
    bool asBool(const FB::variant& a);
    long asInt(const FB::variant& a);
    double asDouble(const FB::variant& a);

    std::string listArray(const std::vector<std::string>&);
    FB::VariantList reverseArray(const std::vector<std::string>& arr);
    FB::VariantList getObjectKeys(const FB::JSObjectPtr& arr);
    FB::VariantList getObjectValues(const FB::JSObjectPtr& arr);
    FB::VariantMap getUserData();

    // Method add
    long add(long a, long b);
    void testEvent(const std::string& param);

private:
    FB::BrowserHostPtr m_host;
    FB::JSAPIPtr m_simpleMath;

    std::string m_testString;
};

