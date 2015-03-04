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
        JSFunction(const FB::JSAPIWeakPtr& obj, std::string func, const FB::SecurityZone);

        virtual bool HasProperty(std::string propName) const override;
        virtual bool HasMethod(std::string methodName) const override;
        virtual variantPromise Invoke(std::string methodName, const std::vector<variant>& args) override;
        virtual variantPromise exec(const std::vector<variant>& args);
        virtual variantPromise call(const std::vector<variant>& args);
        virtual variantPromise apply(const std::vector<variant>& args);

    private:
        void init();

    protected:
        const FB::JSAPIWeakPtr m_apiWeak;
        const std::string m_methodName;
    };
};
    
#endif // JSFunction_h__

