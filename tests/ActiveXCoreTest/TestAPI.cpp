
/**********************************************************\
Original Author: Richard Bateman

Created:    Aug 9, 2010
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
#include "variant_list.h"

#include "TestAPI.h"

TestAPI::TestAPI(FB::BrowserHostPtr host) : m_host(host)
{
    registerMethod("add",  make_method(this, &TestAPI::add));
    registerMethod("echo",  make_method(this, &TestAPI::echo));
    registerMethod("asString",  make_method(this, &TestAPI::asString));
    registerMethod("asBool",  make_method(this, &TestAPI::asBool));
    registerMethod("asInt",  make_method(this, &TestAPI::asInt));
    registerMethod("asDouble",  make_method(this, &TestAPI::asDouble));
    registerMethod("listArray",  make_method(this, &TestAPI::listArray));
    registerMethod("reverseArray",  make_method(this, &TestAPI::reverseArray));
    registerMethod("getUserData",  make_method(this, &TestAPI::getUserData));
    registerMethod("getObjectKeys",  make_method(this, &TestAPI::getObjectKeys));
    registerMethod("getObjectValues",  make_method(this, &TestAPI::getObjectValues));
    registerMethod("testEvent",  make_method(this, &TestAPI::testEvent));

    // Read-write property
    registerProperty("testString",
                     make_property(this,
                        &TestAPI::get_testString,
                        &TestAPI::set_testString));
    // Read-only property
    registerProperty("someInt",
                     make_property(this,
                        &TestAPI::get_someInt));

    registerEvent("onfired");
}

TestAPI::~TestAPI()
{
    //std::map<int,int>::capacity()
}

// Read/Write property someInt
std::string TestAPI::get_testString()
{
    return m_testString;
}
void TestAPI::set_testString(const std::string& val)
{
    m_testString = val;
}

// Read-only property someInt
long TestAPI::get_someInt()
{
    m_host->htmlLog("Returning some int");
    return 12;
}

// add Method
long TestAPI::add(long a, long b)
{
    m_host->htmlLog("Adding two numbers");
    return a+b;
}

// test firing an event
void TestAPI::testEvent(const std::string& param)
{
    this->FireEvent("onfired", FB::variant_list_of(param));
}

FB::variant TestAPI::echo(const FB::variant& a)
{
    m_host->htmlLog("Echoing: " + a.convert_cast<std::string>());
    return a;
}

std::string TestAPI::asString(const FB::variant& a)
{
    return a.convert_cast<std::string>();
}

bool TestAPI::asBool(const FB::variant& a)
{
    return a.convert_cast<bool>();
}

long TestAPI::asInt(const FB::variant& a)
{
    return a.convert_cast<long>();
}

double TestAPI::asDouble(const FB::variant& a)
{
    return a.convert_cast<double>();
}

FB::VariantList TestAPI::reverseArray(const std::vector<std::string>& arr)
{
    FB::VariantList outArr;
    for (std::vector<std::string>::const_reverse_iterator it = arr.rbegin(); it != arr.rend(); it++)
    {
        outArr.push_back(*it);
    }
    return outArr;
}

FB::VariantList TestAPI::getObjectKeys(const FB::JSObjectPtr& arr)
{
    FB::VariantList outArr;
    std::map<std::string, FB::variant> inMap;
    arr->GetObjectValues(arr, inMap);

    for (std::map<std::string, FB::variant>::iterator it = inMap.begin(); it != inMap.end(); it++) {
        outArr.push_back(it->first);
    }
    return outArr;
}

FB::VariantList TestAPI::getObjectValues(const FB::JSObjectPtr& arr)
{
    FB::VariantList outArr;
    std::map<std::string, FB::variant> inMap;
    arr->GetObjectValues(arr, inMap);

    for (std::map<std::string, FB::variant>::iterator it = inMap.begin(); it != inMap.end(); it++) {
        outArr.push_back(it->second);
    }
    return outArr;
}

std::string TestAPI::listArray(const std::vector<std::string>& arr)
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

FB::VariantMap TestAPI::getUserData()
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

