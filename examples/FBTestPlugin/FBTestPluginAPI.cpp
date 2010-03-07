/**********************************************************\
Original Author: Richard Bateman and Georg Fritzsche

Created:    December 3, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass Inc, Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#include "BrowserObjectAPI.h"
#include "DOM/JSAPI_DOMDocument.h"
#include "variant_list.h"
#include "SimpleMathAPI.h"

#include "FBTestPluginAPI.h"

FBTestPluginAPI::FBTestPluginAPI(FB::BrowserHost host) : m_host(host)
{
    registerMethod("add",  make_method(this, &FBTestPluginAPI::add));
    registerMethod("echo",  make_method(this, &FBTestPluginAPI::echo));
    registerMethod("asString",  make_method(this, &FBTestPluginAPI::asString));
    registerMethod("asBool",  make_method(this, &FBTestPluginAPI::asBool));
    registerMethod("asInt",  make_method(this, &FBTestPluginAPI::asInt));
    registerMethod("asDouble",  make_method(this, &FBTestPluginAPI::asDouble));
    registerMethod("listArray",  make_method(this, &FBTestPluginAPI::listArray));
    registerMethod("reverseArray",  make_method(this, &FBTestPluginAPI::reverseArray));
    registerMethod("getUserData",  make_method(this, &FBTestPluginAPI::getUserData));
    registerMethod("getObjectKeys",  make_method(this, &FBTestPluginAPI::getObjectKeys));
    registerMethod("getObjectValues",  make_method(this, &FBTestPluginAPI::getObjectValues));
    registerMethod("testEvent",  make_method(this, &FBTestPluginAPI::testEvent));
    registerMethod("testStreams",  make_method(this, &FBTestPluginAPI::testStreams));

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

	registerEvent("onfired");

    m_simpleMath = new SimpleMathAPI(m_host);
}

FBTestPluginAPI::~FBTestPluginAPI()
{
    //std::map<int,int>::capacity()
}

// Read/Write property someInt
std::string FBTestPluginAPI::get_testString()
{
    return m_testString;
}
void FBTestPluginAPI::set_testString(std::string val)
{
    m_testString = val;
}

// Read-only property someInt
long FBTestPluginAPI::get_someInt()
{
    m_host->htmlLog("Returning some int");
    return 12;
}

// add Method
long FBTestPluginAPI::add(long a, long b)
{
    m_host->htmlLog("Adding two numbers");
    return a+b;
}

// test firing an event
void FBTestPluginAPI::testEvent(std::string param)
{
	this->FireEvent("onfired", FB::variant_list_of(param));
}

FB::variant FBTestPluginAPI::echo(FB::variant a)
{
    m_host->htmlLog("Echoing: " + a.convert_cast<std::string>());
    return a;
}

std::string FBTestPluginAPI::asString(FB::variant a)
{
    return a.convert_cast<std::string>();
}

bool FBTestPluginAPI::asBool(FB::variant a)
{
    return a.convert_cast<bool>();
}

long FBTestPluginAPI::asInt(FB::variant a)
{
    return a.convert_cast<long>();
}

double FBTestPluginAPI::asDouble(FB::variant a)
{
    return a.convert_cast<double>();
}

FB::VariantList FBTestPluginAPI::reverseArray(std::vector<std::string> arr)
{
    FB::VariantList outArr;
    for (std::vector<std::string>::reverse_iterator it = arr.rbegin(); it != arr.rend(); it++)
    {
        outArr.push_back(*it);
    }
    return outArr;
}

FB::VariantList FBTestPluginAPI::getObjectKeys(FB::JSObject arr)
{
    FB::VariantList outArr;
    std::map<std::string, FB::variant> inMap;
    arr->GetObjectValues(arr, inMap);

    for (std::map<std::string, FB::variant>::iterator it = inMap.begin(); it != inMap.end(); it++) {
        outArr.push_back(it->first);
    }
    return outArr;
}

FB::VariantList FBTestPluginAPI::getObjectValues(FB::JSObject arr)
{
    FB::VariantList outArr;
    std::map<std::string, FB::variant> inMap;
    arr->GetObjectValues(arr, inMap);

    for (std::map<std::string, FB::variant>::iterator it = inMap.begin(); it != inMap.end(); it++) {
        outArr.push_back(it->second);
    }
    return outArr;
}

std::string FBTestPluginAPI::listArray(std::vector<std::string> arr)
{
    std::string outStr;
    bool start(true);
    for (std::vector<std::string>::iterator it = arr.begin(); it != arr.end(); it++)
    {
        if (!start) {
            outStr += ", ";
        }
        start = false;
        outStr += *it;
    }
    return outStr;
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
    map["Kids"] = kids;
    return map;
}

FB::JSOutObject FBTestPluginAPI::get_simpleMath()
{
    return m_simpleMath;
}


#include "SimpleStreams.h"

// test streams
bool FBTestPluginAPI::testStreams()
{
	StreamsTest test( m_host );
	return test.run();
}