/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Dec 9, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#include "variant_list.h"
#include "../precompiled_headers.h" // On windows, everything above this line in PCH
#include "Node.h"
#include "Element.h"
#include "Document.h"

#include "Window.h"

using namespace FB::DOM;

Window::Window(const FB::JSObjectPtr& element) : Node(element)
{
}

Window::~Window()
{
}

DocumentPtr Window::getDocument() const
{
    JSObjectPtr api = getProperty<JSObjectPtr>("document");
    return Document::create(api);
}

void Window::alert(const std::wstring& str) const
{
    alert(FB::wstring_to_utf8(str));
}

void Window::alert(const std::string& str) const
{
    callMethod<void>("alert", variant_list_of(str));
}

FB::JSObjectPtr Window::createArray() const
{
    JSObjectPtr arr = this->callMethod<JSObjectPtr>("Array", FB::VariantList());
    return arr;
}

FB::JSObjectPtr Window::createMap() const
{
    JSObjectPtr arr = this->callMethod<JSObjectPtr>("Object", FB::VariantList());
    return arr;
}

std::string Window::getLocation() const
{
    return getNode("location")->getProperty<std::string>("href");
}

//FB::JSObjectPtr FB::DOM::Window::createDate( const std::string& datestring ) const
//{
//    // This is not working; I'm leaving it here in hopes that we can find a way to make it work.
//    // My best idea so far is to inject a help function into javascript :-/
//    try {
//        JSObjectPtr obj;
//        if (datestring.empty())
//            obj = getJSObject()->Construct("Date", FB::VariantList());
//        else
//            obj = getJSObject()->Construct("Date", FB::variant_list_of(datestring));
//        return obj;
//    } catch (...) {
//        return FB::JSObjectPtr();
//    }
//}

