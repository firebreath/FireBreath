/**********************************************************\

  Auto-generated @{PLUGIN_ident}API.cpp

\**********************************************************/

#include "BrowserObjectAPI.h"
#include "variant_list.h"
#include "DOM/JSAPI_DOMDocument.h"

#include "@{PLUGIN_ident}API.h"

@{PLUGIN_ident}API::@{PLUGIN_ident}API(FB::BrowserHost host) : m_host(host)
{
    registerMethod("echo",      make_method(this, &@{PLUGIN_ident}API::echo));
    registerMethod("testEvent", make_method(this, &@{PLUGIN_ident}API::testEvent));

    // Read-write property
    registerProperty("testString",
                     make_property(this,
                        &@{PLUGIN_ident}API::get_testString,
                        &@{PLUGIN_ident}API::set_testString));

    // Read-only property
    registerProperty("version",
                     make_property(this,
                        &@{PLUGIN_ident}API::get_version));
    
    
    registerEvent("onfired");    
}

@{PLUGIN_ident}API::~@{PLUGIN_ident}API()
{
}

// Read/Write property testString
std::string @{PLUGIN_ident}API::get_testString()
{
    return m_testString;
}
void @{PLUGIN_ident}API::set_testString(const std::string& val)
{
    m_testString = val;
}

// Read-only property version
std::string @{PLUGIN_ident}API::get_version()
{
    return "CURRENT_VERSION";
}

// Method echo
FB::variant @{PLUGIN_ident}API::echo(const FB::variant& msg)
{
    return msg;
}

void @{PLUGIN_ident}API::testEvent(const FB::variant& var)
{
    FireEvent("onfired", FB::variant_list_of(var));
}

