/**********************************************************\

  Auto-generated @{PLUGIN.ident}API.cpp

\**********************************************************/

#include "BrowserObjectAPI.h"
#include "variant_list.h"
#include "DOM/JSAPI_DOMDocument.h"

#include "@{PLUGIN.ident}API.h"

@{PLUGIN.ident}API::@{PLUGIN.ident}API(FB::BrowserHostWrapper *host) : m_host(host)
{
    registerMethod("echo",  make_method(this, &@{PLUGIN.ident}API::echo));

    // Read-write property
    registerProperty("testString",
                     make_property(this,
                        &@{PLUGIN.ident}API::get_testString,
                        &@{PLUGIN.ident}API::set_testString));

    // Read-only property
    registerProperty("version",
                     make_property(this,
                        &@{PLUGIN.ident}API::get_version));
}

@{PLUGIN.ident}API::~@{PLUGIN.ident}API()
{
}

// Read/Write property testString
std::string @{PLUGIN.ident}API::get_testString()
{
    return m_testString;
}
void @{PLUGIN.ident}API::set_testString(std::string val)
{
    m_testString = val;
}

// Read-only property version
std::string @{PLUGIN.ident}API::get_version()
{
    return "CURRENT_VERSION";
}

// Method echo
FB::variant @{PLUGIN.ident}API::echo(FB::variant msg)
{
    return msg;
}
