/**********************************************************\ 
 Original Author: Georg Fritzsche
 
 Created:    Aug 2, 2010
 License:    Dual license model; choose one of two:
 New BSD License
 http://www.opensource.org/licenses/bsd-license.php
 - or -
 GNU Lesser General Public License, version 2.1
 http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 2010 Georg Fritzsche, Firebreath development team
 \**********************************************************/

#pragma once
#ifndef JSCALLBACK_H
#define JSCALLBACK_H

#include "JSAPIAuto.h"

namespace FB 
{
    class JSCallback : public FB::JSAPIAuto
    {
    public:
        JSCallback(const FB::CallMethodFunctor& func) 
          : m_jsapi()
        {
            registerMethod("", func);
        }
        
        JSCallback(const FB::CallMethodFunctor& func, FB::JSAPI* jsapi) 
          : m_jsapi(jsapi->shared_ptr())
        {
            registerMethod("", func);
        }
        
    private:
        FB::JSAPIPtr m_jsapi;
    };
    
    template<class T, class F>
    FB::JSAPIPtr make_callback(T* instance, F member_function, bool strong_reference = true)
    {
        if (strong_reference) {
            return FB::JSAPIPtr(new JSCallback(FB::make_method(instance, member_function), instance));
        } else {
            return FB::JSAPIPtr(new JSCallback(FB::make_method(instance, member_function)));
        }
    }
}

#endif
