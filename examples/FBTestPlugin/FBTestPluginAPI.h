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
#include "Timer.h"
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include <boost/weak_ptr.hpp>
#include <boost/optional.hpp>
#include "SimpleStreamHelper.h"
#include "FBPointers.h"

FB_FORWARD_PTR(ThreadRunnerAPI);
FB_FORWARD_PTR(SimpleMathAPI);
FB_FORWARD_PTR(FBTestPlugin);

class FBTestPluginAPI : public FB::JSAPIAuto
{
public:
    FBTestPluginAPI(const FBTestPluginPtr& plugin, const FB::BrowserHostPtr& host);
    virtual ~FBTestPluginAPI();

    FB_JSAPI_EVENT(fired, 1, (const std::string));

    FBTestPluginPtr getPlugin();
    
    ThreadRunnerAPIPtr createThreadRunner();

    std::wstring say(const std::wstring& val);
    // Read/Write property testString
    std::string get_testString();
    void set_testString(const std::string& val);

    // Read-only property someInt
    long get_someInt();

    boost::weak_ptr<SimpleMathAPI> get_simpleMath();

    SimpleMathAPIPtr createSimpleMath();
  
    FB::variant echo(const FB::variant& a);

    std::string asString(const FB::variant& a);
    bool asBool(const FB::variant& a);
    long asInt(const FB::variant& a);
    double asDouble(const FB::variant& a);
    const boost::optional<std::string> optionalTest(const std::string& test1, const boost::optional<std::string>& str);

    std::string charArray(const std::vector<char>& arr);
    std::string listArray(const std::vector<std::string>&);
    FB::VariantList reverseArray(const std::vector<std::string>& arr);
    FB::VariantList getObjectKeys(const FB::JSObjectPtr& arr);
    FB::VariantList getObjectValues(const FB::JSObjectPtr& arr);
    FB::VariantMap getUserData();
    FB::VariantList getUserArray();
    long countArrayLength(const FB::JSObjectPtr &jso); 

    // Method add
    long add(long a, long b);
    void testEvent(const std::string& param);

    // Method to start the streams test
    bool testStreams();

    // Method to test getting a tag from the page
    FB::variant getTagAttribute(const std::wstring &tagName, const long idx, const std::wstring &attribute);

    std::string getPageLocation();
    
    std::string get_pluginPath();
    
    void eval(std::string str);
    FB::variant addWithSimpleMath(const FB::JSObjectPtr& jso, long a, long b);
    void getURL(const std::string& url, const FB::JSObjectPtr& callback);
    void postURL(const std::string& url, const std::string& postdata, const FB::JSObjectPtr& callback);
    void getURLCallback(const FB::JSObjectPtr& callback, bool success, const FB::HeaderMap& headers,
        const boost::shared_array<uint8_t>& data, const size_t size);
	void SetTimeout(const FB::JSObjectPtr& callback, long timeout);
    FB::VariantMap systemHelpersTest();
	void timerCallback(const FB::JSObjectPtr& callback);

    FB::VariantMap getProxyInfo(const boost::optional<std::string>& url);

    void openPopup();

private:
    FB::BrowserHostPtr m_host;
    SimpleMathAPIPtr m_simpleMath;
    FBTestPluginWeakPtr m_pluginWeak;
    std::vector<boost::shared_ptr<FB::Timer> > timers;

    std::string m_testString;
};

