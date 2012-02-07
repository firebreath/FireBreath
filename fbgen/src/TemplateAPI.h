/**********************************************************\

  Auto-generated @{PLUGIN_ident}API.h

\**********************************************************/

#include <string>
#include <sstream>
#include <boost/weak_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "@{PLUGIN_ident}.h"

#ifndef H_@{PLUGIN_ident}API
#define H_@{PLUGIN_ident}API

class @{PLUGIN_ident}API : public FB::JSAPIAuto
{
public:
    ////////////////////////////////////////////////////////////////////////////
    /// @@fn @{PLUGIN_ident}API::@{PLUGIN_ident}API(const @{PLUGIN_ident}Ptr& plugin, const FB::BrowserHostPtr host)
    ///
    /// @@brief  Constructor for your JSAPI object.
    ///         You should register your methods, properties, and events
    ///         that should be accessible to Javascript from here.
    ///
    /// @@see FB::JSAPIAuto::registerMethod
    /// @@see FB::JSAPIAuto::registerProperty
    /// @@see FB::JSAPIAuto::registerEvent
    ////////////////////////////////////////////////////////////////////////////
    @{PLUGIN_ident}API(const @{PLUGIN_ident}Ptr& plugin, const FB::BrowserHostPtr& host) :
        m_plugin(plugin), m_host(host)
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
    virtual ~@{PLUGIN_ident}API() {};

    @{PLUGIN_ident}Ptr getPlugin();

    // Read/Write property ${PROPERTY.ident}
    std::string get_testString();
    void set_testString(const std::string& val);

    // Read-only property ${PROPERTY.ident}
    std::string get_version();

    // Method echo
    FB::variant echo(const FB::variant& msg);
    
    // Event helpers
    FB_JSAPI_EVENT(test, 0, ());
    FB_JSAPI_EVENT(echo, 2, (const FB::variant&, const int));

    // Method test-event
    void testEvent();

private:
    @{PLUGIN_ident}WeakPtr m_plugin;
    FB::BrowserHostPtr m_host;

    std::string m_testString;
};

#endif // H_@{PLUGIN_ident}API

