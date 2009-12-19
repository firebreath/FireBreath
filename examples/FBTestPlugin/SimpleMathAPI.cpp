/**********************************************************\
Original Author: Richard Bateman and Georg Fritzsche

Created:    December 3, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass Inc, Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#include "BrowserObjectAPI.h"
#include <boost/assign.hpp>
#include "DOM/JSAPI_DOMDocument.h"
using boost::assign::list_of;

#include "SimpleMathAPI.h"

SimpleMathAPI::SimpleMathAPI(FB::BrowserHost host) : m_host(host)
{
    registerMethod("add",  make_method(this, &SimpleMathAPI::add));
    registerMethod("sub",  make_method(this, &SimpleMathAPI::sub));
    registerMethod("mult",  make_method(this, &SimpleMathAPI::mult));
    registerMethod("div",  make_method(this, &SimpleMathAPI::div));
}

SimpleMathAPI::~SimpleMathAPI()
{
}

// add Method
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
