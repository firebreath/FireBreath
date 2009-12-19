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

#include "BrowserObjectAPI.h"
#include <boost/assign.hpp>
#include "DOM/JSAPI_DOMDocument.h"
using boost::assign::list_of;

#include "FBTestPluginAPI.h"

FBTestPluginAPI::FBTestPluginAPI(FB::BrowserHostWrapper *host) : m_host(host)
{
    registerMethod("add",  make_method(this, &FBTestPluginAPI::add));

    // Read-write property
    registerProperty("testString",
                     make_property(this, 
                        &FBTestPluginAPI::get_testString,
                        &FBTestPluginAPI::set_testString));

    // Read-only property
    registerProperty("someInt", 
                     make_property(this,
                        &FBTestPluginAPI::get_someInt));
}

FBTestPluginAPI::~FBTestPluginAPI()
{
}

// Read/Write property someInt
std::string FBTestPluginAPI::get_testString()
{
    return m_testString;
}
void FBTestPluginAPI::set_testString(std::string val)
{
    m_testString = val;
}

// Read-only property someInt
int FBTestPluginAPI::get_someInt()
{
    return 12;
}

// add Method
int FBTestPluginAPI::add(int a, int b)
{
    return a+b;
}
