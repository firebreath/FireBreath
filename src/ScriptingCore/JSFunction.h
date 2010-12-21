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
    class JSFunction : public FB::JSAPIAuto {
    public:
        JSFunction(const FB::JSAPIWeakPtr& obj, const std::wstring& func);
        JSFunction(const FB::JSAPIWeakPtr& obj, const std::string& func);

        void init();

        variant exec(const FB::CatchAll& args);

    protected:
        const std::string m_methodName;
        const FB::JSAPIWeakPtr m_apiWeak;
    };
};
    
#endif // JSFunction_h__