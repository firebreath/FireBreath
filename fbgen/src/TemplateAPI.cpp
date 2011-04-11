/**********************************************************\

  Auto-generated @{PLUGIN_ident}API.cpp

\**********************************************************/

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"

#include "@{PLUGIN_ident}API.h"

///////////////////////////////////////////////////////////////////////////////
/// @@fn @{PLUGIN_ident}API::@{PLUGIN_ident}API(const @{PLUGIN_ident}Ptr& plugin, const FB::BrowserHostPtr host)
///
/// @@brief  Constructor for your JSAPI object.  You should register your methods, properties, and events
///         that should be accessible to Javascript from here.
///
/// @@see FB::JSAPIAuto::registerMethod
/// @@see FB::JSAPIAuto::registerProperty
/// @@see FB::JSAPIAuto::registerEvent
///////////////////////////////////////////////////////////////////////////////
@{PLUGIN_ident}API::@{PLUGIN_ident}API(const @{PLUGIN_ident}Ptr& plugin, const FB::BrowserHostPtr& host) : m_plugin(plugin), m_host(host)
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
}

///////////////////////////////////////////////////////////////////////////////
/// @@fn @{PLUGIN_ident}API::~@{PLUGIN_ident}API()
///
/// @@brief  Destructor.  Remember that this object will not be released until
///         the browser is done with it; this will almost definitely be after
///         the plugin is released.
///////////////////////////////////////////////////////////////////////////////
@{PLUGIN_ident}API::~@{PLUGIN_ident}API()
{
}

///////////////////////////////////////////////////////////////////////////////
/// @@fn @{PLUGIN_ident}Ptr @{PLUGIN_ident}API::getPlugin()
///
/// @@brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
@{PLUGIN_ident}Ptr @{PLUGIN_ident}API::getPlugin()
{
    @{PLUGIN_ident}Ptr plugin(m_plugin.lock());
    if (!plugin) {
        throw FB::script_error("The plugin is invalid");
    }
    return plugin;
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
    static int n(0);
    fire_echo(msg, n++);
    return msg;
}

void @{PLUGIN_ident}API::testEvent(const FB::variant& var)
{
    fire_fired(var, true, 1);
}

