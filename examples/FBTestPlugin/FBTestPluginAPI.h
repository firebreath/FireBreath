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
#include <boost/weak_ptr.hpp>
#include "ThreadRunnerAPI.h"

class FBTestPlugin;

class FBTestPluginAPI : public FB::JSAPIAuto
{
public:
    FBTestPluginAPI(boost::shared_ptr<FBTestPlugin> plugin, FB::BrowserHostPtr host);
    virtual ~FBTestPluginAPI();

    boost::shared_ptr<FBTestPlugin> getPlugin() { return m_pluginWeak.lock(); }
    
    FB::JSAPIPtr createThreadRunner() { return FB::JSAPIPtr(new ThreadRunnerAPI(m_host)); }

    std::wstring say(const std::wstring& val);
    // Read/Write property testString
    std::string get_testString();
    void set_testString(const std::string& val);

    // Read-only property someInt
    long get_someInt();

    FB::JSAPIPtr get_simpleMath();
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
    FB::VariantList getUserArray();

    // Method add
    long add(long a, long b);
    void testEvent(const std::string& param);

    // Method to start the streams test
    bool testStreams();

    // Method to test getting a tag from the page
    FB::variant getTagAttribute(const std::wstring &tagName, const long idx, const std::wstring &attribute);

    std::string getPageLocation();
    
    std::string get_pluginPath();

private:
    FB::BrowserHostPtr m_host;
    FB::JSAPIPtr m_simpleMath;
    boost::weak_ptr<FBTestPlugin> m_pluginWeak;

    std::string m_testString;
};
