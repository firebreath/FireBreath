/**********************************************************\

  Auto-generated EchoTestPluginAPI.cpp

\**********************************************************/

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"
#include "global/config.h"
#include <future>

#include "EchoTestPluginAPI.h"

///////////////////////////////////////////////////////////////////////////////
/// @fn FB::variant EchoTestPluginAPI::echo(const FB::variant& msg)
///
/// @brief  Echos whatever is passed from Javascript.
///         Go ahead and change it. See what happens!
///////////////////////////////////////////////////////////////////////////////
FB::variant EchoTestPluginAPI::echo(const FB::variant& msg)
{
    static int n(0);
    fire_echo("So far, you clicked this many times: ", n++);

    // return "foobar";
    return msg;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn FB::variant EchoTestPluginAPI::echo(const FB::variant& msg)
///
/// @brief  Echos whatever is passed from Javascript but asynchronously;
///         This is a great example of how async resolution works
///////////////////////////////////////////////////////////////////////////////
FB::variantPromise EchoTestPluginAPI::echoSlowly(const FB::variant& a)
{
    FB::variantDeferred dfd;

    auto callback = [dfd, a]() {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        dfd.resolve(a);
    };

    std::async(callback);

    return dfd.promise();
}


///////////////////////////////////////////////////////////////////////////////
/// @fn EchoTestPluginPtr EchoTestPluginAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
EchoTestPluginPtr EchoTestPluginAPI::getPlugin()
{
    EchoTestPluginPtr plugin(m_plugin.lock());
    if (!plugin) {
        throw FB::script_error("The plugin is invalid");
    }
    return plugin;
}

FB::VariantList EchoTestPluginAPI::reverseArray(const std::vector<std::string>& arr)
{
    FB::VariantList outArr;
    for (std::vector<std::string>::const_reverse_iterator it = arr.rbegin(); it != arr.rend(); it++)
    {
        outArr.emplace_back(*it);
    }
    return outArr;
}
FB::VariantList EchoTestPluginAPI::getObjectKeys(FB::VariantMap inMap)
{
    FB::VariantList outArr;

    for (auto c : inMap) {
        outArr.emplace_back(c.first);
    }
    return outArr;
}
FB::VariantList EchoTestPluginAPI::getObjectValues(FB::VariantMap inMap)
{
    FB::VariantList outArr;
    for (auto c : inMap) {
        outArr.emplace_back(c.second);
    }
    return outArr;
}

FB::VariantMap EchoTestPluginAPI::echoObject(FB::VariantMap inMap)
{
    return inMap;
}

FB::VariantList EchoTestPluginAPI::echoArray(FB::VariantList inList)
{
    return inList;
}

// Read/Write property testString
std::string EchoTestPluginAPI::get_testString()
{
    return m_testString;
}

void EchoTestPluginAPI::set_testString(const std::string& val)
{
    m_testString = val;
}

// Read-only property version
std::string EchoTestPluginAPI::get_version()
{
    return FBSTRING_PLUGIN_VERSION;
}

void EchoTestPluginAPI::testEvent()
{
    fire_test();
}
