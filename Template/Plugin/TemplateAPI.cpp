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

#include "${API.ident}API.h"

${API.ident}API::${API.ident}API(FB::BrowserHostWrapper *host) : m_host(host)
{
    registerMethod("${METHOD.ident}",  make_method(this, &${API.ident}API::${METHOD.ident}));

    // Read-write property
    registerProperty("${PROPERTY.ident}",
                     make_property(this,
                        &${API.ident}API::get_${PROPERTY.ident},
                        &${API.ident}API::set_${PROPERTY.ident}));

    // Read-only property
    registerProperty("${PROPERTY.ident}",
                     make_property(this,
                        &${API.ident}API::get_${PROPERTY.ident}));
}

${API.ident}API::~${API.ident}API()
{
}

// Read/Write property ${PROPERTY.ident}
${PROPERTY.type} ${API.ident}API::get_${PROPERTY.ident}()
{
    return m_${PROPERTY.ident};
}
void ${API.ident}API::set_${PROPERTY.ident}(${PROPERTY.type} val)
{
    m_${PROPERTY.ident} = val;
}

// Read-only property ${PROPERTY.ident}
${PROPERTY.type} ${API.ident}API::get_${PROPERTY.ident}()
{
}

// Method ${METHOD.ident}
${METHOD.type} ${API.ident}API::${METHOD.ident}(
        ${METHOD.params.0} a
        , ${METHOD.params.1} b
        )
{
    return ;
}
