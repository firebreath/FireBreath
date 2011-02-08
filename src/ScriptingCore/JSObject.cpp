/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 17, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "JSObject.h"

using namespace FB;

JSObject::JSObject(BrowserHostPtr h) :
    host(h)
{
}

JSObject::~JSObject(void)
{
}

void JSObject::SetPropertyAsync(const std::string& propertyName, const variant& value)
{
    host->ScheduleOnMainThread(shared_ptr(), boost::bind((FB::SetPropertyType)&JSAPI::SetProperty, this, propertyName, value));
}

void JSObject::InvokeAsync(const std::string& methodName, const std::vector<variant>& args)
{
    host->ScheduleOnMainThread(shared_ptr(), boost::bind((FB::InvokeType)&JSAPI::Invoke, this, methodName, args));
}

//FB::JSObjectPtr FB::JSObject::Construct( const std::wstring& memberName, const std::vector<variant>& args )
//{
//    return Construct(FB::wstring_to_utf8(memberName), args);
//}

