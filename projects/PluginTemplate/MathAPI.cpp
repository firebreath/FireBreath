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

#include "MathAPI.h"
#include "BrowserObjectAPI.h"
#include <boost/assign.hpp>

using boost::assign::list_of;

MathAPI::MathAPI(FB::BrowserHostWrapper *host) : m_host(host)
{
    registerMethod("returnString",  make_method(this, &MathAPI::returnString));
    registerMethod("intToString",   make_method(this, &MathAPI::intToString));
    registerMethod("sumOf",         make_method(this, &MathAPI::sumOf));
    registerMethod("concatenate",   make_method(this, &MathAPI::concatenate));
    registerMethod("concatMany",    make_method(this, &MathAPI::concatenate2));
    registerMethod("getType",       make_method(this, &MathAPI::getType));
    registerMethod("getElementHTML",make_method(this, &MathAPI::getElementHTML));

    registerProperty("message",
                     make_property(this, 
                        &MathAPI::get_Message,
                        &MathAPI::set_Message));
    registerProperty("readOnlyMessage", 
                     make_property(this,
                        &MathAPI::get_ReadOnlyMessage));
}

MathAPI::~MathAPI()
{
}

std::string MathAPI::getElementHTML(const std::string& elemId)
{
    FB::AutoPtr<FB::BrowserObjectAPI> doc = m_host->getDOMDocument();

    FB::AutoPtr<FB::BrowserObjectAPI> elem = doc->Invoke("getElementById",
        FB::VariantList( list_of(elemId) ) )
        .cast<FB::AutoPtr<FB::BrowserObjectAPI>>();

    return elem->GetProperty("innerHTML").cast<std::string>();
}

std::string MathAPI::returnString(const std::string& s)
{
    return s;
}

std::string MathAPI::intToString(int i)
{
    std::stringstream ss;
    ss << i;
    return ss.str();
}

int MathAPI::sumOf(int a, int b)
{
    return a+b;
}

std::string MathAPI::concatenate(const std::string& a, const std::string& b, const std::string& c)
{
    return a+b+c;
}

std::string MathAPI::concatenate2(const std::string& a, const FB::VariantList& list)
{
    FB::VariantList::const_iterator it(list.begin());
    std::string s(a);

    for( ; it != list.end(); ++it)
        s += it->convert_cast<std::string>();

    return s;
}

void MathAPI::set_Message(const std::string& s)
{
    m_message = s;
}

std::string MathAPI::get_Message()
{
    return m_message;
}

std::string MathAPI::get_ReadOnlyMessage()
{
    return m_readOnlyMessage;
}

std::string MathAPI::getType(const FB::variant a)
{
    return a.get_type().name();
}