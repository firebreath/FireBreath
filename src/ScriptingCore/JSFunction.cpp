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

FB::JSFunction::JSFunction( const FB::JSAPIWeakPtr& obj, const std::wstring& func )
    : m_apiWeak(obj), m_methodName(FB::wstring_to_utf8(func)), FB::JSAPIAuto(FB::wstring_to_utf8(func) + "()")
{
    init();
}

FB::JSFunction::JSFunction( const FB::JSAPIWeakPtr& obj, const std::string& func )
    : m_apiWeak(obj), m_methodName(func), FB::JSAPIAuto(func + "()")
{
    init();
}

void FB::JSFunction::init()
{
    // There are no reserved members on this object
    m_reservedMembers.clear();
    // For now we'll just allow dynamic attributes; later we may expand this somehow
    m_allowDynamicAttributes = true;
    registerMethod("", make_method(this, &FB::JSFunction::exec));
}

FB::variant FB::JSFunction::exec( const FB::CatchAll& args )
{
    FB::JSAPIPtr api = m_apiWeak.lock();
    if (!api)
        throw new FB::script_error("Invalid JSAPI object");
    return api->Invoke(m_methodName, args.value);
}
