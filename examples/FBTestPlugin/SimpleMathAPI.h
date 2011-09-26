/**********************************************************\
Original Author: Richard Bateman and Georg Fritzsche

Created:    December 3, 2009
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

class SimpleMathAPI : public FB::JSAPIAuto
{
public:
    SimpleMathAPI(const FB::BrowserHostPtr& host);
    virtual ~SimpleMathAPI();

    // Read/Write property testString
    std::string get_testString();
    void set_testString(const std::string& val);

    // Read-only property someInt
    int get_someInt();

    // Read-only self (property, method)
    const FB::JSAPIPtr get_self();
    const FB::JSAPIPtr GetSelf();

    // Method add
    long add(long a, long b);
    long sub(long a, long b);
    long mult(long a, long b);
    long div(long a, long b);
    FB::VariantList getArray();

private:
    FB::BrowserHostPtr m_host;

    std::string m_testString;
};

