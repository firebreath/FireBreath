/**********************************************************\
Original Author: Richard Bateman and Georg Fritzsche

Created:    December 3, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass Inc, Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#include "JSObject.h"
#include "DOM/Document.h"
#include "variant_list.h"

#include "SimpleMathAPI.h"

SimpleMathAPI::SimpleMathAPI(FB::BrowserHostPtr host) : m_host(host)
{
    registerMethod("add",  make_method(this, &SimpleMathAPI::add));
    registerMethod("sub",  make_method(this, &SimpleMathAPI::sub));
    registerMethod("mult",  make_method(this, &SimpleMathAPI::mult));
    registerMethod("div",  make_method(this, &SimpleMathAPI::div));
    registerProperty("getArray",  make_property(this, &SimpleMathAPI::getArray));

    registerProperty("self",  make_property(this, &SimpleMathAPI::get_self));
    registerMethod("GetSelf",  make_method(this, &SimpleMathAPI::GetSelf));
}

SimpleMathAPI::~SimpleMathAPI()
{
}

// add Method
FB::VariantList SimpleMathAPI::getArray() 
{
    std::vector<std::string> ret;
    ret.emplace_back("aa");
    ret.emplace_back("bb");
    ret.emplace_back("cc");
    ret.emplace_back("dd");
    return FB::make_variant_list(ret);
}
long SimpleMathAPI::add(long a, long b)
{
    return a+b;
}
long SimpleMathAPI::sub(long a, long b)
{
    return a-b;
}
long SimpleMathAPI::mult(long a, long b)
{
    return a*b;
}
long SimpleMathAPI::div(long a, long b)
{
    return a/b;
}

const FB::JSAPIPtr SimpleMathAPI::get_self()
{
    return shared_from_this();
}

const FB::JSAPIPtr SimpleMathAPI::GetSelf()
{
    return shared_from_this();
}
