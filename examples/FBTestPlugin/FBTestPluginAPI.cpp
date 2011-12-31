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

#include "JSObject.h"
#include "DOM/Document.h"
#include "DOM/Window.h"
#include "variant_list.h"
#include "FBTestPlugin.h"
#include "SimpleMathAPI.h"
#include "ThreadRunnerAPI.h"
#include "SimpleStreams.h"
#include "SystemHelpers.h"
#include <boost/make_shared.hpp>

#include "FBTestPluginAPI.h"

FBTestPluginAPI::FBTestPluginAPI(const FBTestPluginPtr& plugin, const FB::BrowserHostPtr& host) : m_host(host), m_pluginWeak(plugin)
{    
    registerMethod("add",  make_method(this, &FBTestPluginAPI::add));
    registerMethod(L"echo",  make_method(this, &FBTestPluginAPI::echo));
    registerMethod(L"eval",  make_method(this, &FBTestPluginAPI::eval));
    registerMethod(L"asString",  make_method(this, &FBTestPluginAPI::asString));
    registerMethod(L"asBool",  make_method(this, &FBTestPluginAPI::asBool));
    registerMethod(L"asInt",  make_method(this, &FBTestPluginAPI::asInt));
    registerMethod("asDouble",  make_method(this, &FBTestPluginAPI::asDouble));
    registerMethod("charArray",  make_method(this, &FBTestPluginAPI::charArray));
    registerMethod("listArray",  make_method(this, &FBTestPluginAPI::listArray));
    registerMethod("reverseArray",  make_method(this, &FBTestPluginAPI::reverseArray));
    registerMethod("getUserData",  make_method(this, &FBTestPluginAPI::getUserData));
    registerMethod("getUserArray",  make_method(this, &FBTestPluginAPI::getUserArray));
    registerMethod("getObjectKeys",  make_method(this, &FBTestPluginAPI::getObjectKeys));
    registerMethod("getObjectValues",  make_method(this, &FBTestPluginAPI::getObjectValues));
    registerMethod("testEvent",  make_method(this, &FBTestPluginAPI::testEvent));
    registerMethod("testStreams",  make_method(this, &FBTestPluginAPI::testStreams));
    registerMethod("getTagAttribute", make_method(this, &FBTestPluginAPI::getTagAttribute));
    registerMethod("getPageLocation", make_method(this, &FBTestPluginAPI::getPageLocation));
    registerMethod("createThreadRunner", make_method(this, &FBTestPluginAPI::createThreadRunner));
    registerMethod("optionalTest", make_method(this, &FBTestPluginAPI::optionalTest));
    registerMethod("getURL", make_method(this, &FBTestPluginAPI::getURL));
    registerMethod("postURL", make_method(this, &FBTestPluginAPI::postURL));
    registerMethod("openPopup", make_method(this, &FBTestPluginAPI::openPopup));
    registerMethod("setTimeout",  make_method(this, &FBTestPluginAPI::SetTimeout));
    registerMethod("systemHelpersTest", make_method(this, &FBTestPluginAPI::systemHelpersTest));
    registerMethod(L"скажи",  make_method(this, &FBTestPluginAPI::say));
    
    registerMethod("addWithSimpleMath", make_method(this, &FBTestPluginAPI::addWithSimpleMath));
    registerMethod("createSimpleMath", make_method(this, &FBTestPluginAPI::createSimpleMath));

    registerMethod("countArrayLength",  make_method(this, &FBTestPluginAPI::countArrayLength));
    // Read-write property
    registerProperty("testString",
                     make_property(this,
                        &FBTestPluginAPI::get_testString,
                        &FBTestPluginAPI::set_testString));

    registerProperty("simpleMath",
                     make_property(this,
                        &FBTestPluginAPI::get_simpleMath));
    // Read-only property
    registerProperty("someInt",
                     make_property(this,
                        &FBTestPluginAPI::get_someInt));
    registerProperty("pluginPath",
                     make_property(this, &FBTestPluginAPI::get_pluginPath));

    registerMethod("getProxyInfo", make_method(this, &FBTestPluginAPI::getProxyInfo));

    m_simpleMath = boost::make_shared<SimpleMathAPI>(m_host);
}

FBTestPluginAPI::~FBTestPluginAPI()
{
    //std::map<int,int>::capacity()
}

FBTestPluginPtr FBTestPluginAPI::getPlugin()
{
    FBTestPluginPtr plugin = m_pluginWeak.lock();
    if (!plugin)
        throw FB::script_error("The plugin object has been destroyed");
    return plugin;
}

std::wstring FBTestPluginAPI::say(const std::wstring& val)
{
    return L"вот, я говорю \"" + val + L"\"";
}

// Read/Write property someInt
std::string FBTestPluginAPI::get_testString()
{
    return m_testString;
}
void FBTestPluginAPI::set_testString(const std::string& val)
{
    m_testString = val;
}

// Read-only property someInt
long FBTestPluginAPI::get_someInt()
{
    m_host->htmlLog("Returning some int");
    return 12;
}

void FBTestPluginAPI::eval(std::string str)
{
    m_host->evaluateJavaScript(str);
}

// add Method
long FBTestPluginAPI::add(long a, long b)
{
    m_host->htmlLog("Adding two numbers");
    return a+b;
}

// test firing an event
void FBTestPluginAPI::testEvent(const std::string& param)
{
    fire_fired(param);
}

FB::variant FBTestPluginAPI::echo(const FB::variant& a)
{
    try {
        if (a.is_of_type<FB::JSObjectPtr>()) {
            m_host->htmlLog("Echoing: " + a.cast<FB::JSObjectPtr>()->Invoke("ToString", FB::variant_list_of()).convert_cast<std::string>());
        } else {
            m_host->htmlLog("Echoing: " + a.convert_cast<std::string>());
        }
    } catch (...) {
        m_host->htmlLog("Echoing: (unknown)");
    }
    
    return a;
}

std::string FBTestPluginAPI::asString(const FB::variant& a)
{
    return a.convert_cast<std::string>();
}

bool FBTestPluginAPI::asBool(const FB::variant& a)
{
    return a.convert_cast<bool>();
}

long FBTestPluginAPI::asInt(const FB::variant& a)
{
    return a.convert_cast<long>();
}

double FBTestPluginAPI::asDouble(const FB::variant& a)
{
    return a.convert_cast<double>();
}

FB::VariantList FBTestPluginAPI::reverseArray(const std::vector<std::string>& arr)
{
    FB::VariantList outArr;
    for (std::vector<std::string>::const_reverse_iterator it = arr.rbegin(); it != arr.rend(); it++)
    {
        outArr.push_back(*it);
    }
    return outArr;
}

FB::VariantList FBTestPluginAPI::getObjectKeys(const FB::JSObjectPtr& arr)
{
    FB::VariantList outArr;
    std::map<std::string, FB::variant> inMap;
    arr->GetObjectValues(arr, inMap);

    for (std::map<std::string, FB::variant>::iterator it = inMap.begin(); it != inMap.end(); it++) {
        outArr.push_back(it->first);
    }
    return outArr;
}

FB::VariantList FBTestPluginAPI::getObjectValues(const FB::JSObjectPtr& arr)
{
    FB::VariantList outArr;
    std::map<std::string, FB::variant> inMap;
    arr->GetObjectValues(arr, inMap);

    for (std::map<std::string, FB::variant>::iterator it = inMap.begin(); it != inMap.end(); it++) {
        outArr.push_back(it->second);
    }
    return outArr;
}

std::string FBTestPluginAPI::charArray(const std::vector<char>& arr)
{
    std::string outStr;
    bool start(true);
    for (std::vector<char>::const_iterator it = arr.begin(); it != arr.end(); it++)
    {
        if (!start) {
            outStr += ", ";
        }
        start = false;
        outStr += *it;
    }
    return outStr;
}

std::string FBTestPluginAPI::listArray(const std::vector<std::string>& arr)
{
    std::string outStr;
    bool start(true);
    for (std::vector<std::string>::const_iterator it = arr.begin(); it != arr.end(); it++)
    {
        if (!start) {
            outStr += ", ";
        }
        start = false;
        outStr += *it;
    }
    return outStr;
}

FB::VariantList FBTestPluginAPI::getUserArray()
{
    FB::VariantList map;
    map.push_back("Richard Bateman");
    map.push_back("Somewhere in Utah");
    map.push_back("Hazel");
    map.push_back("Brown");
    FB::VariantList kids;
    kids.push_back("Caleb");
    kids.push_back("Unknown");
    kids.push_back("Ok, I only have one, but I'm filling space");
    FB::VariantMap innerMap;
    innerMap["test"] = 12;
    innerMap["test2"] = true;
    innerMap["test3"] = 12.4;
    innerMap["test4"] = "asdf";
    map.push_back(innerMap);
    kids.push_back(innerMap);
    map.push_back(kids);
    return map;
}

FB::VariantMap FBTestPluginAPI::getUserData()
{
    FB::VariantMap map;
    map["Name"] = "Richard Bateman";
    map["Location"] = "Somewhere in Utah";
    map["EyeColor"] = "Hazel";
    map["HairColor"] = "Brown";
    FB::VariantList kids;
    kids.push_back("Caleb");
    kids.push_back("Unknown");
    kids.push_back("Ok, I only have one, but I'm filling space");
    FB::VariantMap innerMap;
    innerMap["test"] = 12;
    innerMap["test2"] = true;
    innerMap["test3"] = 12.4;
    innerMap["test4"] = "asdf";
    map["inner"] = innerMap;
    kids.push_back(innerMap);
    map["Kids"] = kids;
    return map;
}

boost::weak_ptr<SimpleMathAPI> FBTestPluginAPI::get_simpleMath()
{
    return m_simpleMath;
}

FB::variant FBTestPluginAPI::getTagAttribute(const std::wstring &tagName, const long idx, const std::wstring &attribute)
{
    std::vector<FB::DOM::ElementPtr> tagList = m_host->getDOMDocument()->getElementsByTagName(tagName);
    if (!tagList.size()) {
        return "No matching tags found";
    }
    return tagList[idx]->getJSObject()->GetProperty(attribute);
}

std::string FBTestPluginAPI::getPageLocation()
{
    return m_host->getDOMWindow()->getLocation();
}

// test streams
bool FBTestPluginAPI::testStreams()
{
    StreamsTest test( m_host );
    return test.run();
}

std::string FBTestPluginAPI::get_pluginPath()
{
    return getPlugin()->getPluginPath();
}

long FBTestPluginAPI::countArrayLength(const FB::JSObjectPtr &jso) 
{
    if (!jso)
        throw FB::invalid_arguments();
    if (!jso->HasProperty("getArray"))
        throw FB::invalid_arguments();
    FB::VariantList array = jso->GetProperty("getArray").cast<FB::VariantList>();
    long len = array.size();// array->GetProperty("length").convert_cast<long>();
    return len;
}
FB::variant FBTestPluginAPI::addWithSimpleMath(const FB::JSObjectPtr& math, long a, long b) 
{
    return math->Invoke("add", FB::variant_list_of(a)(b));
}

ThreadRunnerAPIPtr FBTestPluginAPI::createThreadRunner()
{
    return boost::make_shared<ThreadRunnerAPI>(m_host, m_pluginWeak);
}

void FBTestPluginAPI::getURL(const std::string& url, const FB::JSObjectPtr& callback)
{
    FB::SimpleStreamHelper::AsyncGet(m_host, FB::URI::fromString(url),
        boost::bind(&FBTestPluginAPI::getURLCallback, this, callback, _1, _2, _3, _4));
}

void FBTestPluginAPI::postURL(const std::string& url, const std::string& postdata, const FB::JSObjectPtr& callback)
{
    FB::SimpleStreamHelper::AsyncPost(m_host, FB::URI::fromString(url), postdata,
        boost::bind(&FBTestPluginAPI::getURLCallback, this, callback, _1, _2, _3, _4));
}

void FBTestPluginAPI::getURLCallback(const FB::JSObjectPtr& callback, bool success,
    const FB::HeaderMap& headers, const boost::shared_array<uint8_t>& data, const size_t size) {
    FB::VariantMap outHeaders;
    for (FB::HeaderMap::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        if (headers.count(it->first) > 1) {
            if (outHeaders.find(it->first) != outHeaders.end()) {
                outHeaders[it->first].cast<FB::VariantList>().push_back(it->second);
            } else {
                outHeaders[it->first] = FB::VariantList(FB::variant_list_of(it->second));
            }
        } else {
            outHeaders[it->first] = it->second;
        }
    }
    if (success) {
        std::string dstr(reinterpret_cast<const char*>(data.get()), size);
        callback->InvokeAsync("", FB::variant_list_of
            (true)
            (outHeaders)
            (dstr)
            );
    } else {
        callback->InvokeAsync("", FB::variant_list_of(false));
    }
}

void FBTestPluginAPI::SetTimeout(const FB::JSObjectPtr& callback, long timeout)
{
	bool recursive = false;
	FB::TimerPtr timer = FB::Timer::getTimer(timeout, recursive, boost::bind(&FBTestPluginAPI::timerCallback, this, callback));
	timer->start();
	timers.push_back(timer);
}

void FBTestPluginAPI::timerCallback(const FB::JSObjectPtr& callback)
{
	callback->Invoke("", FB::variant_list_of());
	// TODO: delete This timer
}

FB::VariantMap FBTestPluginAPI::systemHelpersTest(){
    FB::VariantMap result;
    
    result["homedir"] = FB::System::getHomeDirPath();
    result["tempdir"] = FB::System::getTempPath();
    result["appdata"] = FB::System::getAppDataPath("FBTestPlugin");
    result["appdata_local"] = FB::System::getLocalAppDataPath("FBTestPlugin");

    return result;
}

const boost::optional<std::string> FBTestPluginAPI::optionalTest( const std::string& test1, const boost::optional<std::string>& str )
{
    if (str)
        m_host->htmlLog(*str);
    else
        m_host->htmlLog("No string passed in!");
    return str;
}

SimpleMathAPIPtr FBTestPluginAPI::createSimpleMath()
{
    // Create a new simplemath object each time
    return boost::make_shared<SimpleMathAPI>(m_host);
}

FB::VariantMap FBTestPluginAPI::getProxyInfo(const boost::optional<std::string>& url)
{
    std::map<std::string, std::string> proxyInfo;
    
    if (url)
        m_host->DetectProxySettings(proxyInfo, *url);
    else
        m_host->DetectProxySettings(proxyInfo, "http://www.firebreath.org");
    FB::VariantMap outMap;
    for (std::map<std::string, std::string>::iterator it = proxyInfo.begin();
        it != proxyInfo.end(); ++it) {
        outMap[it->first] = it->second;
    }
    return outMap;
}

void FBTestPluginAPI::openPopup()
{
    m_host->Navigate("http://www.firebreath.org", "_blank");
}
