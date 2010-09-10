/**********************************************************\

  Auto-generated @{PLUGIN.ident}API.cpp

\**********************************************************/

#include "BrowserObjectAPI.h"
#include "variant_list.h"
#include "DOM/JSAPI_DOMDocument.h"

#include "@{PLUGIN.ident}API.h"

@{PLUGIN.ident}API::@{PLUGIN.ident}API(FB::BrowserHost host) : m_host(host)
{
    registerMethod("echo",      make_method(this, &@{PLUGIN.ident}API::echo));
    registerMethod("testEvent", make_method(this, &@{PLUGIN.ident}API::testEvent));

    // Read-write property
    registerProperty("testString",
                     make_property(this,
                        &@{PLUGIN.ident}API::get_testString,
                        &@{PLUGIN.ident}API::set_testString));

    // Read-only property
    registerProperty("version",
                     make_property(this,
                        &@{PLUGIN.ident}API::get_version));
    
    
    registerEvent("onfired");    
}

@{PLUGIN.ident}API::~@{PLUGIN.ident}API()
{
}

// Read/Write property testString
std::string @{PLUGIN.ident}API::get_testString()
{
    return m_testString;
}
void @{PLUGIN.ident}API::set_testString(const std::string& val)
{
    m_testString = val;
}

// Read-only property version
std::string @{PLUGIN.ident}API::get_version()
{
    return "CURRENT_VERSION";
}

// Method echo
FB::variant @{PLUGIN.ident}API::echo(const FB::variant& msg)
{
    return msg;
}

void @{PLUGIN.ident}API::testEvent(const FB::variant& var)
{
    FireEvent("onfired", FB::variant_list_of(var));
}

