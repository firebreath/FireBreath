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
          : m_jsapi(jsapi)
        {
            registerMethod("", func);
        }
        
    private:
        FB::AutoPtr<FB::JSAPI> m_jsapi;
    };
    
    struct strong_reference_type {};
    struct weak_reference_type {};
    extern const strong_reference_type strong_reference;
    extern const weak_reference_type weak_reference;
    
    template<class T, class F>
    FB::AutoPtr<FB::JSAPI> make_callback(T* instance, F member_function, bool strong_reference = true)
    {
        if (strong_reference) {
            return new JSCallback(FB::make_method(instance, member_function), instance);
        } else {
            return new JSCallback(FB::make_method(instance, member_function));
        }
    }
}

#endif
