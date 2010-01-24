/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Jan 23, 2010
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "BrowserObjectAPI.h"
#include "DOM/JSAPI_DOMWindow.h"
#include "variant_list.h"

#include "BrowserHostWrapper.h"

void FB::BrowserHostWrapper::htmlLog(std::string str)
{
    try {
        printf("Logging: %s\n", str.c_str());
        FB::JSAPI_DOMWindow window = getDOMWindow();

        FB::JSObject obj = window.getProperty<FB::JSObject>("console");
        obj->invoke("log", FB::variant_list_of(str));
    } catch (std::exception &e) {
        printf("Exception: %s", e.what());
        // Fail silently; logging should not require success.
        return;
    }
}
