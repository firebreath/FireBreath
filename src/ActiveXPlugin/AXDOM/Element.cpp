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

ElementImpl::ElementImpl(const FB::JSObject& element, IHTMLElement *el)
    : FB::DOM::ElementImpl(element), m_axElement(el)
{
}

ElementImpl::~ElementImpl()
{
}
