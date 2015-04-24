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

#include "Deferred.h"
#include "JSFunction.h"
#include <memory>

FB::JSFunction::JSFunction( const FB::JSAPIWeakPtr& obj, const std::wstring& func, const FB::SecurityZone zone)
    : FB::JSAPIAuto(zone, FB::wstring_to_utf8(func) + "()"), m_apiWeak(obj), m_methodName(FB::wstring_to_utf8(func))
{
    init();
}

FB::JSFunction::JSFunction( const FB::JSAPIWeakPtr& obj, std::string func, const FB::SecurityZone zone)
    : FB::JSAPIAuto(zone, func + "()"), m_apiWeak(obj), m_methodName(func)
{
    init();
}

void FB::JSFunction::init()
{
}

FB::variantPromise FB::JSFunction::exec( const std::vector<variant>& args )
{
    FB::JSAPIPtr api = m_apiWeak.lock();
    if (!api)
        throw FB::script_error("Invalid JSAPI object");
    // Force calls to use the zone this function was created with
    FB::scoped_zonelock _l(api, getZone());
    return api->Invoke(m_methodName, args);
}

FB::variantPromise FB::JSFunction::call( const std::vector<variant>& args )
{
    FB::VariantList list;
    if (args.size() >= 1) {
        list.insert(list.end(), args.begin()+1, args.end());
    }
    return exec(list);
}

FB::variantPromise FB::JSFunction::apply( const std::vector<variant>& args )
{
    if (args.size() >= 2) {
        auto fn = std::dynamic_pointer_cast<FB::JSFunction>(shared_from_this());
        return args[1].convert_cast<FB::VariantList>().thenPipe<variant>([fn](VariantList list) {
            return fn->exec(list);
        });
    } else {
        return exec(VariantList{});
    }
}


bool FB::JSFunction::HasMethod( std::string methodName ) const
{
    if (methodName == "" || methodName == "apply" || methodName == "call") {
        return true;
    } else {
        return FB::JSAPIAuto::HasMethod(methodName);
    }
}

FB::variantPromise FB::JSFunction::Invoke( std::string methodName, const std::vector<variant>& args )
{
    if (methodName == "") {
        return exec(args);
    } else if (methodName == "call") {
        return call(args);
    } else if (methodName == "apply") {
        return apply(args);
    } else {
        return FB::JSAPIAuto::Invoke(methodName, args);
    }
}

bool FB::JSFunction::HasProperty( std::string propName ) const
{
    if (propName == "call" || propName == "apply")
        return false;
    else
        return FB::JSAPIAuto::HasProperty(propName);
}




