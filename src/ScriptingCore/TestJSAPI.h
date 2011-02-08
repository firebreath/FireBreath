/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 1, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_TESTOBJECTJSAPI
#define H_TESTOBJECTJSAPI

#include "JSAPISimple.h"

#define TESTOBJECTJSAPI_ACCESSLISTLENGTH 15

class TestObjectJSAPI : public FB::JSAPISimple
{
public:
    TestObjectJSAPI()
    {
        registerMethod("setValue", (FB::CallMethodPtr)&TestObjectJSAPI::callSetValue);
        registerMethod("getValue", (FB::CallMethodPtr)&TestObjectJSAPI::callGetValue);
        registerMethod("invalidate", (FB::CallMethodPtr)&TestObjectJSAPI::callInvalidate);

        registerProperty("value", (FB::GetPropPtr)&TestObjectJSAPI::getValue, 
                                  (FB::SetPropPtr)&TestObjectJSAPI::setValue);
    }

    virtual ~TestObjectJSAPI()
    {
    }

public:
    FB::variant callSetValue(const std::vector<FB::variant>& args)
    {
        if (args.size() != 2)
            throw FB::invalid_arguments();

        try {
            if (args[0].convert_cast<long>() >= 0
                || args[0].convert_cast<long>() < TESTOBJECTJSAPI_ACCESSLISTLENGTH)
            {
                int i = args[0].convert_cast<long>();
                m_accessList[i] = args[1];
                std::string tmp = args[1].convert_cast<std::string>();
            }
            return true;
        } catch (...) {
            throw FB::script_error("Could not convert parameter");
        }
    }

    FB::variant callGetValue(const std::vector<FB::variant>& args)
    {
        if (args.size() != 1)
            throw FB::invalid_arguments();

        try {
            if (args[0].convert_cast<long>() >= 0
                || args[0].convert_cast<long>() < TESTOBJECTJSAPI_ACCESSLISTLENGTH)
            {
                int i = args[0].convert_cast<long>();
                return m_accessList[i];
            }
        } catch (...) {
            throw FB::script_error("Could not convert parameter");
        }

        throw FB::invalid_arguments();
    }

    FB::variant callInvalidate(const std::vector<FB::variant>& args)
    {
        invalidate();
        return FB::variant();
    }

    FB::variant getValue()
    {
        return propValue;
    }

    void setValue(const FB::variant& value)
    {
        propValue = value;
    }

protected:
    FB::variant m_accessList[TESTOBJECTJSAPI_ACCESSLISTLENGTH];
    FB::variant propValue;
};

#endif

