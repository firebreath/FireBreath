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

class {{API_NAME}}API : public FB::JSAPIAuto
{
public:
    {{API_NAME}}API(FB::BrowserHostWrapper *host);
    virtual ~{{API_NAME}}API();

    // Read/Write property {{PROPERTY_NAME}}
    {{PROPERTY_TYPE}} get_{{PROPERTY_NAME}}();
    void set_{{PROPERTY_NAME}}({{PROPERTY_TYPE}} val);

    // Read-only property {{PROPERTY_NAME}}
    {{PROPERTY_TYPE}} get_{{PROPERTY_NAME}}();

    // Method {{METHOD_NAME}}
    {{RETURN_TYPE}} {{METHOD_NAME}}(
            {{PARAM_TYPE}} a
            , {{PARAM_TYPE}} b
            );

private:
    FB::AutoPtr<FB::BrowserHostWrapper> m_host;

    {{PROPERTY_TYPE}} m_{{PROPERTY_NAME}};
};
