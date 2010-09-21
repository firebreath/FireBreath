/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sep 21, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Facebook, Inc and the Firebreath development team
\**********************************************************/

#ifndef H_AXDOM_DOCUMENT
#define H_AXDOM_DOCUMENT

#include <string>
#include "Win/win_common.h"
#include <atlctl.h>
#include "BrowserObjectAPI.h"
#include "DOM/Document.h"
#include "DOM/Element.h"

namespace AXDOM {

    /**
     * DocumentImpl - used as Document (which is a shared_ptr)
     *
     * Provides a wrapper around a BrowserObjectAPI * that represents a DOM document
     **/
    class DocumentImpl : public FB::DOM::DocumentImpl
    {
    public:
        DocumentImpl(const FB::JSObject &element, FB::DOM::Window &win, IHTMLDocument2 *doc);
        virtual ~DocumentImpl();

    public:
        virtual FB::DOM::Window getWindow();
        virtual FB::DOM::Element getElementById(const std::string& elem_id);
        virtual std::vector<FB::DOM::Element> getElementsByTagName(const std::string& tagName);

    protected:
        FB::DOM::Window m_win;
        CComQIPtr<IHTMLDocument2> m_htmlDoc;
    };

};

#endif // H_AXDOM_DOCUMENT