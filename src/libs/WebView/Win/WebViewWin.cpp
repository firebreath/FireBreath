/**********************************************************\
Original Author: Vijaye Raji

Created:    Jun 23, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2011 Facebook, Inc
\**********************************************************/


#include "WebViewWin.h"
#include "PluginEvents/AttachedEvent.h"
#include "Win/PluginWindowWin.h"
#include <stdexcept>

using namespace FB::View;

WebViewPtr WebView::create( const FB::PluginCorePtr& plugin, const FB::BrowserHostPtr& parentHost )
{
    WebViewPtr ptr(boost::make_shared<WebViewWin>(plugin, parentHost));
    ptr->init();
    return ptr;
}

WebViewWin::WebViewWin( const FB::PluginCorePtr& plugin, const FB::BrowserHostPtr& parentHost )
    : WebView(plugin, parentHost), oleStorage(new OleStorage())
{
}

WebViewWin::~WebViewWin()
{
    if (fbBrowserHostPtr) {
        fbBrowserHostPtr->shutdown();
        fbBrowserHostPtr.reset();
    }
}

void WebViewWin::loadUri( const FB::URI& uri )
{
    HRESULT hr = webBrowser->Navigate(BSTR(FB::utf8_to_wstring(uri.toString()).c_str()), NULL, NULL, NULL, NULL);
    assert(SUCCEEDED(hr));

    hr = webBrowser->put_Visible(VARIANT_TRUE);
    assert(SUCCEEDED(hr));
}

void WebViewWin::loadHtml( const std::string& html )
{
    if (this->webBrowser == NULL)
        throw std::runtime_error("Internal Web Browser not initialized");

    std::wstring content = FB::utf8_to_wstring(html);
    closePage();

    IHTMLDocument2* htmlDoc = GetDocument();
    assert(htmlDoc);
    if (!htmlDoc) throw std::runtime_error("Could not get access to document element in internal web browser");

    SAFEARRAY* safeArray;
    SAFEARRAYBOUND arrayBound = {1, 0};
    safeArray = SafeArrayCreate(VT_VARIANT, 1, &arrayBound);

    VARIANT* var;
    HRESULT hr = SafeArrayAccessData(safeArray, (void**)&var);
    if (FAILED(hr)) {
        SafeArrayDestroy(safeArray);
        throw std::runtime_error("Could not access COM array");
    }

    var->vt = VT_BSTR;
    var->bstrVal = SysAllocString(content.c_str());

    htmlDoc->write(safeArray);

    // It is really strange that we have to do this here.
    initPage(); // Initialize the page, create browserhost
    InjectCustomObjects(); // Inject custom variables into the window object

    htmlDoc->close();
    htmlDoc->Release();

    SysFreeString(var->bstrVal);
    SafeArrayDestroy(safeArray);

    // We call this with false because we already created the browserhost and injected objects
    OnDocumentReady(false);
}

void WebViewWin::closePage()
{
    HRESULT hr = this->webBrowser->Navigate(L"about:blank", NULL, NULL, NULL, NULL);
    assert(SUCCEEDED(hr));
}

bool WebViewWin::onWindowAttached( FB::AttachedEvent *evt, FB::PluginWindowWin *wnd )
{
    hostWindow = wnd->getHWND();
    boost::scoped_ptr<OleClientSite> cSite(new OleClientSite());
    clientSite.swap(cSite);
    clientSite->inplaceSite->SetHostWindow(hostWindow);
    ConstructBrowser();
    return false;
}

bool WebViewWin::onWindowDetached( FB::DetachedEvent *evt, FB::PluginWindowWin *wnd )
{
    closePage();

    browserObject->SetClientSite(NULL);
    webBrowser.Release();
    browserObject->Close(OLECLOSE_NOSAVE);
    browserObject.Release();

    return false;
}

HRESULT WebViewWin::ConstructBrowser()
{
    HRESULT hr;
    IOleWindow* oleWindow = NULL;

    hr = OleCreate(
        CLSID_WebBrowser,
        IID_IOleObject,
        OLERENDER_DRAW,
        0,
        this->clientSite.get(),
        oleStorage.get(),
        (void**)&browserObject);
    if (FAILED(hr)) goto Error;

    hr = this->browserObject->SetClientSite(this->clientSite.get());
    if (FAILED(hr)) goto Error;

    hr = browserObject->SetHostNames(L"FireBreath Plugin", NULL);
    if (FAILED(hr)) goto Error;

    hr = OleSetContainedObject(browserObject, TRUE);
    if (FAILED(hr)) goto Error;

    RECT rect;
    GetClientRect(this->hostWindow, &rect);
    hr = browserObject->DoVerb(
        OLEIVERB_SHOW,
        NULL,
        this->clientSite.get(),
        -1,
        this->hostWindow,
        &rect);
    if (FAILED(hr)) goto Error;

    hr = browserObject->QueryInterface(&this->webBrowser);
    if (FAILED(hr)) goto Error;

    hr = this->browserObject->QueryInterface(&oleWindow);
    if (FAILED(hr)) goto Error;

    oleWindow->GetWindow(&this->browserWindow);
    this->Resize(rect);

    IConnectionPointContainer* connPtContainer;
    hr = this->webBrowser->QueryInterface(&connPtContainer);
    if (SUCCEEDED(hr)) {
        IConnectionPoint* connPt;
        hr = connPtContainer->FindConnectionPoint(
            DIID_DWebBrowserEvents2,
            &connPt);
        if (SUCCEEDED(hr)) {
            WebBrowserEventHandler* eventHandler =
                new WebBrowserEventHandler(this);
            DWORD cookie = 0;
            hr = connPt->Advise(eventHandler, &cookie);
        }
    }

    if (oleWindow) oleWindow->Release();
    return S_OK;

Error:
    if (oleWindow) oleWindow->Release();
    if (browserObject) browserObject.Release();
    if (webBrowser) webBrowser.Release();
    return hr;
}

void WebViewWin::initPage()
{
    fbBrowserHostPtr =
        boost::make_shared<FB::ActiveX::ActiveXBrowserHost>(webBrowser, clientSite.get());
    this->fbBrowserHostPtr->initJS(this);
}

void WebViewWin::InjectCustomObjects()
{
    FB::DOM::WindowPtr jsWin(fbBrowserHostPtr->getDOMWindow());

    FB::VariantMap& pageVars(this->getJSVariables());
    try {
        for (FB::VariantMap::iterator it(pageVars.begin()); it != pageVars.end(); ++it) {
            jsWin->setProperty(it->first, it->second);
        }
    } catch (const FB::script_error&) {
        // It is possible that the window won't allow us to set values; in this case,
        // we don't worry about it because we're likely at about:blank
    }
}

void WebViewWin::Resize( RECT rect )
{
    SetWindowPos(
        this->browserWindow,
        NULL,
        0,
        0,
        rect.right,
        rect.bottom,
        SWP_NOACTIVATE | SWP_NOZORDER);
}

IHTMLDocument2* WebViewWin::GetDocument()
{
    IDispatch* dispatch;
    HRESULT hr = this->webBrowser->get_Document(&dispatch);
    if (FAILED(hr)) return NULL;

    IHTMLDocument2* htmlDoc2 = NULL;
    if (dispatch) {
        hr = dispatch->QueryInterface(&htmlDoc2);
        dispatch->Release();
        if (FAILED(hr)) return NULL;
    }

    return htmlDoc2;
}

void WebViewWin::OnDownloadComplete()
{
    // This gets called every time anything (including images) gets downloaded
}

void WebViewWin::OnDocumentReady(const bool pageNeedsInit)
{
    IHTMLDocument2* doc = GetDocument();
    if (!doc)
        return;

    if (pageNeedsInit) {
        initPage();
        InjectCustomObjects();
    }

    ICustomDoc* customDoc;
    HRESULT hr = doc->QueryInterface(&customDoc);
    if (SUCCEEDED(hr)) {
        DocHostUIHandler* uiHandler = new DocHostUIHandler();
        hr = customDoc->SetUIHandler(uiHandler);
        customDoc->Release();
    }

    //FB::DOM::WindowPtr jsWin(fbBrowserHostPtr->getDOMWindow());

    //FB::VariantMap& pageVars(this->getJSVariables());
    //try {
    //    for (FB::VariantMap::iterator it(pageVars.begin()); it != pageVars.end(); ++it) {
    //        jsWin->setProperty(it->first, it->second);
    //    }
    //} catch (const FB::script_error&) {
    //    // It is possible that the window won't allow us to set values; in this case,
    //    // we don't worry about it because we're likely at about:blank
    //}
}

void WebViewWin::OnBeforeNavigate()
{
    if (fbBrowserHostPtr) {
        fbBrowserHostPtr->shutdown();
        fbBrowserHostPtr.reset();
    }
}
