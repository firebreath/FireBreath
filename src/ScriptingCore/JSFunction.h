/**********************************************************\ 
 Original Author: Richard Bateman
 
 Created:    Dec 20, 2010
 License:    Dual license model; choose one of two:
 New BSD License
 http://www.opensource.org/licenses/bsd-license.php
 - or -
 GNU Lesser General Public License, version 2.1
 http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 2010 Richard Bateman, FaceBook inc, Firebreath development team
 \**********************************************************/

#pragma once
#ifndef JSFunction_h__
#define JSFunction_h__

#include "JSAPIAuto.h"

namespace FB {
    FB_FORWARD_PTR(JSFunction);
    class JSFunction : public FB::JSAPIAuto {
    public:
        JSFunction(const FB::JSAPIWeakPtr& obj, const std::wstring& func, const FB::SecurityZone);
        JSFunction(const FB::JSAPIWeakPtr& obj, const std::string& func, const FB::SecurityZone);

        virtual bool HasMethodObject(const std::string& name) const { return false; }
        virtual bool HasProperty(const std::string& propName) const;
        virtual bool HasMethod(const std::string& methodName) const;
        virtual variant Invoke(const std::string& methodName, const std::vector<variant>& args);
        virtual variant exec(const std::vector<variant>& args);
        virtual variant call(const std::vector<variant>& args);
        virtual variant apply(const std::vector<variant>& args);

    private:
        void init();

    protected:
        const FB::JSAPIWeakPtr m_apiWeak;
        const std::string m_methodName;
    };
};
    
#endif // JSFunction_h__

