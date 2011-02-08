/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 16, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_NPOBJECTAPI
#define H_NPOBJECTAPI

#include "NpapiTypes.h"
#include "npruntime.h"
#include "JSAPI.h"
#include "JSObject.h"
#include "NpapiBrowserHost.h"

namespace FB { namespace Npapi {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  NPObjectAPI
    ///
    /// @brief  Provides a FB::JSObject implementation that wraps a NPObject*
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class NPObjectAPI :
        public FB::JSObject
    {
    public:
        NPObjectAPI(NPObject *, NpapiBrowserHostPtr);
        virtual ~NPObjectAPI(void);

        void *getEventId() const { return (void*)obj; }
        void *getEventContext() const { return browser->getContextID(); };
        NPObject *getNPObject() const { return obj; }

        void getMemberNames(std::vector<std::string> &nameVector) const;
        size_t getMemberCount() const;
    public:
        virtual JSAPIPtr getJSAPI() const;

    protected:
        NpapiBrowserHostPtr browser;
        NPObject *obj;
        bool is_JSAPI;
        FB::JSAPIWeakPtr inner;

    public:
        bool HasMethod(const std::string& methodName) const;
        bool HasProperty(const std::string& propertyName) const;
        bool HasProperty(int idx) const;
        bool HasEvent(const std::string& eventName) const;

        variant GetProperty(const std::string& propertyName);
        void SetProperty(const std::string& propertyName, const variant& value);
        variant GetProperty(int idx);
        void SetProperty(int idx, const variant& value);

        variant Invoke(const std::string& methodName, const std::vector<variant>& args);

        //FB::JSObjectPtr Construct(const std::string& memberName, const std::vector<variant>& args);
    };

}; };

#endif

