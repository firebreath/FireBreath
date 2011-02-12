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

bool FB::JSFunction::HasMethod( const std::string& methodName ) const
{
    if (methodName == "") {
        return true;
    } else {
        return FB::JSAPIAuto::HasMethod(methodName);
    }
}

FB::variant FB::JSFunction::Invoke( const std::string& methodName, const std::vector<variant>& args )
{
    if (methodName == "") {
        return exec(args);
    } else {
        return FB::JSAPIAuto::Invoke(methodName, args);
    }
}

