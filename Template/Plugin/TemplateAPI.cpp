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

#include "{{API_NAME}}API.h"

{{API_NAME}}API::{{API_NAME}}API(FB::BrowserHostWrapper *host) : m_host(host)
{
    registerMethod("{{METHOD_NAME}}",  make_method(this, &{{API_NAME}}API::{{METHOD_NAME}}));

    // Read-write property
    registerProperty("{{PROPERTY_NAME}",
                     make_property(this, 
                        &{{API_NAME}}API::get_{{PROPERTY_NAME}},
                        &{{API_NAME}}API::set_{{PROPERTY_NAME}}));

    // Read-only property
    registerProperty("{{PROPERTY_NAME}}", 
                     make_property(this,
                        &{{API_NAME}}API::get_{{PROPERTY_NAME}}));
}

{{API_NAME}}API::~{{API_NAME}}API()
{
}

// Read/Write property {{PROPERTY_NAME}}
{{PROPERTY_TYPE}} {{API_NAME}}API::get_{{PROPERTY_NAME}}()
{
    return m_{{PROPERTY_NAME}};
}
void {{API_NAME}}API::set_{{PROPERTY_NAME}}({{PROPERTY_TYPE}} val)
{
    m_{{PROPERTY_NAME}} = val;
}

// Read-only property {{PROPERTY_NAME}}
{{PROPERTY_TYPE}} {{API_NAME}}API::get_{{PROPERTY_NAME}}()
{
}
