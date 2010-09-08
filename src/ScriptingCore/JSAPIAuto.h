/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    November 7, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#ifndef JSAPIAUTO_H
#define JSAPIAUTO_H

#include "JSAPI.h"
#include "MethodConverter.h"
#include "PropertyConverter.h"
#include <vector>
#include <string>

namespace FB {
    class JSAPIAuto : public JSAPI
    {
    public:
        JSAPIAuto();
        ~JSAPIAuto();

        // Enumerate members
        void getMemberNames(std::vector<std::string> &nameVector);
        size_t getMemberCount();

        // Invoke method
        virtual variant Invoke(const std::string& methodName, const std::vector<variant>& args);

        // Manage methods
        virtual void registerMethod(const std::wstring& name, const CallMethodFunctor& func);
        virtual void registerMethod(const std::string& name, const CallMethodFunctor& func);
        virtual bool HasMethod(const std::string& methodName);
        virtual bool HasProperty(const std::string& propertyName);
        virtual bool HasProperty(int idx);

        // Manage properties
        virtual void registerProperty(const std::wstring& name, const PropertyFunctors& propFuncs);
        virtual void registerProperty(const std::string& name, const PropertyFunctors& propFuncs);
        virtual variant GetProperty(const std::string& propertyName);
        virtual void SetProperty(const std::string& propertyName, const variant& value);
        virtual variant GetProperty(int idx);
        virtual void SetProperty(int idx, const variant& value);

        // Default properties and methods for all JS objects
        virtual std::string ToString();
        virtual bool get_valid();
    private:
        MethodFunctorMap m_methodFunctorMap;
        PropertyFunctorsMap m_propertyFunctorsMap;
    };
};

#endif // JSAPIAUTO_H
