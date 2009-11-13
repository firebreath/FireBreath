/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 29, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_JSAPISIMPLE
#define H_FB_JSAPISIMPLE

#include "jsapi.h"

namespace FB {

    class JSAPISimple :
        public FB::JSAPI
    {
    public:
        JSAPISimple(void);
        virtual ~JSAPISimple(void);

    public:
        // Methods for registering properties and functions to the auto-table
        virtual void registerMethod(std::string name, CallMethodPtr func);
        virtual void registerProperty(std::string name, GetPropPtr getFunc, SetPropPtr setFunc);

        // Methods to query existance of members on the API
        virtual bool HasMethod(std::string methodName);
        virtual bool HasProperty(std::string propertyName);
        virtual bool HasProperty(int idx);

        // Methods to manage properties on the API
        virtual variant GetProperty(std::string propertyName);
        virtual void SetProperty(std::string propertyName, const variant value);
        virtual variant GetProperty(int idx);
        virtual void SetProperty(int idx, const variant value);

        // Methods to manage methods on the API
        virtual variant Invoke(std::string methodName, std::vector<variant>& args);

    public:
        // Example function calls and read-only property; override these if desired in derived classes
        virtual variant callFireEvent(std::vector<variant>&args);
        virtual variant callToString(std::vector<variant>&args);
        virtual variant getValid();

    protected:
        MethodMap m_methodMap;
        PropertyMap m_propertyMap;
    };

};

#endif