/**********************************************************\

  Auto-generated EchoTestPluginAPI.h

\**********************************************************/

#include <string>
#include <sstream>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "EchoTestPlugin.h"

#ifndef H_EchoTestPluginAPI
#define H_EchoTestPluginAPI

// This macro defines EchoTestPluginAPIPtr, EchoTestPluginAPIWeakPtr,
// EchoTestPluginAPIConstPtr, and EchoTestPluginAPIWeakConstPtr
FB_FORWARD_PTR(EchoTestPluginAPI);

class EchoTestPluginAPI : public FB::JSAPIAuto
{
public:
    ////////////////////////////////////////////////////////////////////////////
    /// @fn EchoTestPluginAPI::EchoTestPluginAPI(const EchoTestPluginPtr& plugin, const FB::BrowserHostPtr host)
    ///
    /// @brief  Constructor for your JSAPI object.
    ///         You should register your methods, properties, and events
    ///         that should be accessible to Javascript from here.
    ///
    /// @see FB::JSAPIAuto::registerMethod
    /// @see FB::JSAPIAuto::registerProperty
    /// @see FB::JSAPIAuto::registerEvent
    ////////////////////////////////////////////////////////////////////////////
    EchoTestPluginAPI(const EchoTestPluginPtr& plugin, const FB::BrowserHostPtr& host) :
        m_plugin(plugin), m_host(host)
    {
        registerMethod("echo",      make_method(this, &EchoTestPluginAPI::echo));
        registerMethod("echoSlowly",make_method(this, &EchoTestPluginAPI::echoSlowly));
        registerMethod("reverseArray",make_method(this, &EchoTestPluginAPI::reverseArray));
        registerMethod("getObjectKeys",make_method(this, &EchoTestPluginAPI::getObjectKeys));
        registerMethod("getObjectValues",make_method(this, &EchoTestPluginAPI::getObjectValues));
        registerMethod("echoArray",make_method(this, &EchoTestPluginAPI::echoArray));
        registerMethod("echoObject",make_method(this, &EchoTestPluginAPI::echoObject));
        registerMethod("testEvent", make_method(this, &EchoTestPluginAPI::testEvent));

        // Read-write property
        registerProperty("testString",
                         make_property(this,
                                       &EchoTestPluginAPI::get_testString,
                                       &EchoTestPluginAPI::set_testString));

        // Read-only property
        registerProperty("version",
                         make_property(this,
                                       &EchoTestPluginAPI::get_version));
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @fn EchoTestPluginAPI::~EchoTestPluginAPI()
    ///
    /// @brief  Destructor.  Remember that this object will not be released until
    ///         the browser is done with it; this will almost definitely be after
    ///         the plugin is released.
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~EchoTestPluginAPI() {};

    EchoTestPluginPtr getPlugin();

    // Read/Write property ${PROPERTY.ident}
    std::string get_testString();
    void set_testString(const std::string& val);

    FB::VariantList reverseArray(const std::vector<std::string>& arr);
    FB::VariantList getObjectKeys(FB::VariantMap obj);
    FB::VariantList getObjectValues(FB::VariantMap obj);
    FB::VariantList echoArray(FB::VariantList inList);
    FB::VariantMap echoObject(FB::VariantMap inMap);
    // Read-only property ${PROPERTY.ident}
    std::string get_version();

    // Method echo
    FB::variant echo(const FB::variant& msg);
    // Method echo slowly (async echo)
    FB::variantPromise echoSlowly(const FB::variant& a);

    // Event helpers
    FB_JSAPI_EVENT(test, 0, ());
    FB_JSAPI_EVENT(echo, 2, (const FB::variant&, const int));

    // Method test-event
    void testEvent();

private:
    EchoTestPluginWeakPtr m_plugin;
    FB::BrowserHostPtr m_host;

    std::string m_testString;
};

#endif // H_EchoTestPluginAPI

