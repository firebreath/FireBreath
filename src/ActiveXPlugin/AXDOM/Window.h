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

#ifndef H_AXDOM_WINDOW
#define H_AXDOM_WINDOW

#include "Win/win_common.h"
#include <atlctl.h>
#include <string>
#include <boost/shared_ptr.hpp>
#include "DOM/Window.h"

namespace AXDOM {

    /**
     * Window
     *
     * Provides a wrapper around a BrowserObjectAPI * that represents a DOM element
     **/
    class WindowImpl : public FB::DOM::WindowImpl
    {
    public:
        WindowImpl(FB::JSObject obj, IWebBrowser2 *web);
        virtual ~WindowImpl();

    public:
        virtual FB::DOM::Document getDocument();
        virtual void alert(const std::string& str);

    protected:
        CComPtr<IHTMLWindow2> m_htmlWin;
        CComPtr<IHTMLDocument2> m_htmlDoc;
        CComPtr<IDispatch> m_htmlDocDisp;
        CComPtr<IWebBrowser2> m_webBrowser;
    };

};

#endif // H_AXDOM_WINDOW
