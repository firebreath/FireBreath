/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sep 21, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Facebook, Inc and the Firebreath development team
\**********************************************************/

#include "Element.h"

using namespace AXDOM;

Element::Element(const FB::JSObjectPtr& element, IWebBrowser *web)
    : FB::DOM::Element(element), m_axElement(FB::ptr_cast<IDispatchAPI>(element)->getIDispatch()),
      m_webBrowser(web)
{
    if (!m_axElement)
        throw std::bad_cast("This is not a valid Element");
}

Element::~Element()
{
}
