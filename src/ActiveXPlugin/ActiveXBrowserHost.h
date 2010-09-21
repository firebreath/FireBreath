/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 30, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_ACTIVEXBROWSERHOST
#define H_ACTIVEXBROWSERHOST

#include "Win/win_common.h"
#include "COM_config.h"
#include <atlctl.h>
#include "BrowserHostWrapper.h"
#include "APITypes.h"

class ActiveXBrowserHost;
class IDispatchAPI;
typedef boost::shared_ptr<ActiveXBrowserHost> ActiveXBrowserHostPtr;
#define as_ActiveXBrowserHost(x) boost::dynamic_pointer_cast<ActiveXBrowserHost>(x)
typedef boost::shared_ptr<IDispatchAPI> IDispatchAPIPtr;
#define as_IDispatchAPI(x) boost::dynamic_pointer_cast<IDispatchAPI>(x)

class ActiveXBrowserHost :
    public FB::BrowserHostWrapper
{
public:
    ActiveXBrowserHost(IWebBrowser2 *doc);
    virtual ~ActiveXBrowserHost(void);
    virtual void ScheduleAsyncCall(void (*func)(void *), void *userData);

    virtual void *getContextID();

    virtual FB::BrowserStream* createStream(const std::string& url, FB::PluginEventSink* callback, 
                                            bool cache = true, bool seekable = false, 
                                            size_t internalBufferSize = 128 * 1024 );

    void setWindow(HWND wnd);

public:
    FB::DOM::Document getDOMDocument();
    FB::DOM::Window getDOMWindow();
    void evaluateJavaScript(const std::string &script);

protected:
    void initDOMObjects();
    HWND m_hWnd;
    CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2> m_htmlDoc;
    CComQIPtr<IDispatch, &IID_IDispatch> m_htmlDocDisp;
    CComPtr<IHTMLWindow2> m_htmlWin;
    CComPtr<IWebBrowser2> m_webBrowser;
    CComQIPtr<IDispatch, &IID_IDispatch> m_htmlWinDisp;
    FB::DOM::Window m_window;
    FB::DOM::Document m_document;

public:
    FB::variant getVariant(const VARIANT *cVar);
    void getComVariant(VARIANT *dest, const FB::variant &var);
};

#endif // H_ACTIVEXBROWSERHOST
