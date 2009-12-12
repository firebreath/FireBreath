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

#include <string>
#include <sstream>
#include "JSAPIAuto.h"
#include "BrowserHostWrapper.h"

class ${API.ident}API : public FB::JSAPIAuto
{
public:
    ${API.ident}API(FB::BrowserHostWrapper *host);
    virtual ~${API.ident}API();

    // Read/Write property ${PROPERTY.ident}
    ${PROPERTY.type} get_${PROPERTY.ident}();
    void set_${PROPERTY.ident}(${PROPERTY.type} val);

    // Read-only property ${PROPERTY.ident}
    ${PROPERTY.type} get_${PROPERTY.ident}();

    // Method ${METHOD.ident}
    ${PLUGIN.prefix}{{RETURN_TYPE}} ${METHOD.ident}(
            ${PLUGIN.prefix}{{PARAM_TYPE}} a
            , ${PLUGIN.prefix}{{PARAM_TYPE}} b
            );

private:
    FB::AutoPtr<FB::BrowserHostWrapper> m_host;

    ${PROPERTY.type} m_${PROPERTY.ident};
};
