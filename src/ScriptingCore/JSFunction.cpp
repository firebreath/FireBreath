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

#include "precompiled_headers.h" // On windows, everything above this line in PCH
#include "JSFunction.h"

FB::JSFunction::JSFunction( const FB::JSAPIWeakPtr& obj, const std::wstring& func, const FB::SecurityZone zone)
    : FB::JSAPIAuto(zone, FB::wstring_to_utf8(func) + "()"), m_apiWeak(obj), m_methodName(FB::wstring_to_utf8(func))
{
    init();
}

FB::JSFunction::JSFunction( const FB::JSAPIWeakPtr& obj, const std::string& func, const FB::SecurityZone zone)
    : FB::JSAPIAuto(zone, func + "()"), m_apiWeak(obj), m_methodName(func)
{
    init();
}

void FB::JSFunction::init()
{
    m_allowMethodObjects = false;
    // There are no reserved members on this object
    m_reservedMembers.clear();
}

FB::variant FB::JSFunction::exec( const std::vector<variant>& args )
{
    FB::JSAPIPtr api = m_apiWeak.lock();
    if (!api)
        throw new FB::script_error("Invalid JSAPI object");
    // Force calls to use the zone this function was created with
    FB::scoped_zonelock _l(api, getZone());
    return api->Invoke(m_methodName, args);
}

FB::variant FB::JSFunction::call( const std::vector<variant>& args )
{
    FB::VariantList list;
    if (args.size() >= 1) {
        list.insert(list.end(), args.begin()+1, args.end());
    }
    return exec(list);
}

FB::variant FB::JSFunction::apply( const std::vector<variant>& args )
{
    FB::VariantList list;
    if (args.size() >= 2) {
        list = args[1].convert_cast<FB::VariantList>();
    }
    return exec(list);
}


bool FB::JSFunction::HasMethod( const std::string& methodName ) const
{
    if (methodName == "" || methodName == "apply" || methodName == "call") {
        return true;
    } else {
        return FB::JSAPIAuto::HasMethod(methodName);
    }
}

FB::variant FB::JSFunction::Invoke( const std::string& methodName, const std::vector<variant>& args )
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

bool FB::JSFunction::HasProperty( const std::string& propName ) const
{
    if (propName == "call" || propName == "apply")
        return false;
    else
        return FB::JSAPIAuto::HasProperty(propName);
}




