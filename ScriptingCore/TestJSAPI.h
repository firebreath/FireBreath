/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 1, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_TESTOBJECTJSAPI
#define H_TESTOBJECTJSAPI

#include "JSAPI.h"

#define TESTOBJECTJSAPI_ACCESSLISTLENGTH 15

class TestObjectJSAPI : public FB::JSAPI
{
public:
    TestObjectJSAPI()
    {
        registerMethod("setValue", (FB::CallMethodPtr)&TestObjectJSAPI::callSetValue);
        registerMethod("getValue", (FB::CallMethodPtr)&TestObjectJSAPI::callGetValue);
    }

    virtual ~TestObjectJSAPI()
    {
    }

public:
    FB::variant callSetValue(std::vector<FB::variant>& args)
    {
        if (args.size() != 2)
            throw invalid_arguments();

        try {
            if (args[0].convert_cast<int>() >= 0
                || args[0].convert_cast<int>() < TESTOBJECTJSAPI_ACCESSLISTLENGTH)
            {
                int i = args[0].convert_cast<int>();
                m_accessList[i] = args[1];
                std::string tmp = args[1].convert_cast<std::string>();
            }
            return true;
        } catch (...) {
            return false;
        }
    }

    FB::variant callGetValue(std::vector<FB::variant>& args)
    {
        if (args.size() != 1)
            throw invalid_arguments();

        try {
            if (args[0].convert_cast<int>() >= 0
                || args[0].convert_cast<int>() < TESTOBJECTJSAPI_ACCESSLISTLENGTH)
            {
                int i = args[0].convert_cast<int>();
                return m_accessList[i];
            }
        } catch (...) {
            throw script_error("Could not convert parameter");
        }

        throw invalid_arguments();
    }

    bool callInvalidate(std::vector<FB::variant>& args, FB::variant &retVal)
    {
        invalidate();
    }

    int getRefCount()
    {
        return m_refCount;
    }

protected:
    FB::variant m_accessList[TESTOBJECTJSAPI_ACCESSLISTLENGTH];
};

#endif