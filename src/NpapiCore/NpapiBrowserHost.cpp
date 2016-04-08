/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 15, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include <memory>
#include <boost/config.hpp>
#include <boost/algorithm/string.hpp>

#include "NpapiTypes.h"
#include "APITypes.h"
#include "NpapiPluginModule.h"
#include "NPJavascriptObject.h"
#include "NPObjectAPI.h"
#include "DOM/Document.h"
#include "DOM/Window.h"
#include "Deferred.h"

#include "NpapiStream.h"
#include "NpapiBrowserHost.h"
#include "BrowserStreamRequest.h"

#include "NPVariantUtil.h"
#include "URI.h"

using namespace FB::Npapi;
using FB::Promise;
using FB::VariantList;
using FB::VariantMap;

using boost::algorithm::split;
using boost::algorithm::is_any_of;
using boost::algorithm::istarts_with;
using boost::algorithm::replace_first;
using std::vector;
using std::string;
using std::map;

namespace
{
    template<class T>
    NPVariantBuilderMap::value_type makeBuilderEntry()
    {
        return NPVariantBuilderMap::value_type(&typeid(T), select_npvariant_builder::select<T>());
    }

    NPVariantBuilderMap makeNPVariantBuilderMap()
    {
        NPVariantBuilderMap tdm;
        tdm.insert(makeBuilderEntry<bool>());
        tdm.insert(makeBuilderEntry<char>());
        tdm.insert(makeBuilderEntry<unsigned char>());
        tdm.insert(makeBuilderEntry<short>());
        tdm.insert(makeBuilderEntry<unsigned short>());
        tdm.insert(makeBuilderEntry<int>());
        tdm.insert(makeBuilderEntry<unsigned int>());
        tdm.insert(makeBuilderEntry<long>());
        tdm.insert(makeBuilderEntry<unsigned long>());

#ifndef BOOST_NO_LONG_LONG
        tdm.insert(makeBuilderEntry<long long>());
        tdm.insert(makeBuilderEntry<unsigned long long>());
#endif

        tdm.insert(makeBuilderEntry<float>());
        tdm.insert(makeBuilderEntry<double>());

        tdm.insert(makeBuilderEntry<std::string>());
        tdm.insert(makeBuilderEntry<std::wstring>());

        tdm.insert(makeBuilderEntry<FB::FBNull>());
        tdm.insert(makeBuilderEntry<FB::FBVoid>());
        //tdm.insert(makeBuilderEntry<FB::FBDateString>());
        tdm.insert(makeBuilderEntry<FB::VariantList>());
        tdm.insert(makeBuilderEntry<FB::VariantMap>());
        tdm.insert(makeBuilderEntry<FB::JSAPIPtr>());
        tdm.insert(makeBuilderEntry<FB::JSAPIWeakPtr>());
        tdm.insert(makeBuilderEntry<FB::JSObjectPtr>());
        tdm.insert(makeBuilderEntry<const std::exception>());
        tdm.insert(makeBuilderEntry<const std::exception_ptr>());
        
        return tdm;
    }

    const NPVariantBuilderMap& getNPVariantBuilderMap()
    {
        static const NPVariantBuilderMap tdm = makeNPVariantBuilderMap();
        return tdm;
    }
}

NpapiBrowserHost::NpapiBrowserHost(NpapiPluginModule *module, NPP npp)
    : module(module), m_npp(npp), m_jsHelper(nullptr)
{
    assert(!!module);
    // Initialize NPNFuncs to NULL values
    memset(&NPNFuncs, 0, sizeof(NPNetscapeFuncs));
}

NpapiBrowserHost::~NpapiBrowserHost(void)
{
}

void NpapiBrowserHost::shutdown() {
    memset(&NPNFuncs, 0, sizeof(NPNetscapeFuncs));
    FB::BrowserHost::shutdown();

    // Release these now as the BrowserHost will be expired when the they go out of scope in the destructor.
    m_htmlWin.reset();
    m_htmlElement.reset();
    m_htmlDoc.reset();

    if (m_jsHelper) {
        ReleaseObject(m_jsHelper);
        m_jsHelper = nullptr;
    }
}

bool NpapiBrowserHost::_scheduleAsyncCall(void (*func)(void *), void *userData) const
{
    if (isShutDown())
        return false;
    PluginThreadAsyncCall(func, userData);
    return true;
}

void NpapiBrowserHost::setBrowserFuncs(NPNetscapeFuncs *pFuncs)
{
    copyNPBrowserFuncs(&NPNFuncs, pFuncs, m_npp);

    NPObject *window(nullptr);
    NPObject *element(nullptr);
    try {
        GetValue(NPNVWindowNPObject, (void**)&window);
        GetValue(NPNVPluginElementNPObject, (void**)&element);

        m_htmlWin = NPObjectAPIPtr(new NPObjectAPI(window, std::dynamic_pointer_cast<NpapiBrowserHost>(shared_from_this())));
        m_htmlElement = NPObjectAPIPtr(new NPObjectAPI(element, std::dynamic_pointer_cast<NpapiBrowserHost>(shared_from_this())));
        ReleaseObject(window);
        ReleaseObject(element);
    } catch (...) {
        if (window && !m_htmlWin)
            ReleaseObject(window);
        if (element && !m_htmlElement)
            ReleaseObject(element);
    }
    if (m_htmlWin) {
        m_htmlDoc = std::dynamic_pointer_cast<NPObjectAPI>(m_htmlWin->GetPropertySync("document").cast<FB::JSObjectPtr>());
    }
}

FB::DOM::DocumentPtr NpapiBrowserHost::getDOMDocument()
{
    if (!m_htmlDoc)
        throw std::runtime_error("Cannot find HTML document");

    return FB::DOM::Document::create(m_htmlDoc);
}

FB::DOM::WindowPtr NpapiBrowserHost::getDOMWindow()
{
    if (!m_htmlWin)
        throw std::runtime_error("Cannot find HTML window");

    return FB::DOM::Window::create(m_htmlWin);
}

FB::DOM::ElementPtr NpapiBrowserHost::getDOMElement()
{
    if (!m_htmlElement)
        throw std::runtime_error("Cannot find HTML window");

    return FB::DOM::Element::create(m_htmlElement);
}

Promise<VariantList> NpapiBrowserHost::GetArrayValues(FB::JSObjectPtr obj) {
    NPObjectAPIPtr ptr = std::dynamic_pointer_cast<NPObjectAPI>(obj);
    if (!ptr) {
        return VariantList();
    }

    uint32_t len = ptr->GetPropertySync("length").convert_cast<uint32_t>();
    VariantList out;
    for (size_t i{ 0 }; i < len; ++i) {
        out.emplace_back(ptr->GetPropertySync(i));
    }
    return out;
}
Promise<FB::VariantMap> NpapiBrowserHost::GetObjectValues(FB::JSObjectPtr obj) {
    VariantMap dst;
    NPObjectAPIPtr src = std::dynamic_pointer_cast<NPObjectAPI>(obj);
    if (!src) {
        return dst;
    }
    try {
        vector<string> fields;
        src->getMemberNames(fields);

        for (auto fname : fields) {
            dst[fname] = src->GetPropertySync(fname);
        }
    } catch (const FB::script_error& e) {
        throw e;
    }
    return dst;
}

void NpapiBrowserHost::deferred_release( NPObject* obj )
{
    m_deferredObjects.push(obj);
    if (isMainThread()) {
        DoDeferredRelease();
    }
}

bool isExpired(std::pair<void*, NPObjectWeakRef> cur) {
    return cur.second.expired();
}

void NpapiBrowserHost::DoDeferredRelease() const
{
    assertMainThread();
    NPObject* cur(nullptr);
    while (m_deferredObjects.try_pop(cur)) {
        ReleaseObject(cur);
    }
    // Also remove any expired IDispatch WeakReferences
    NPObjectRefMap::iterator iter = m_cachedNPObject.begin();
    NPObjectRefMap::iterator endIter = m_cachedNPObject.end();
    while (iter != endIter) {
        if (isExpired(*iter))
            m_cachedNPObject.erase(iter++);
        else
            ++iter;
    }
}

void NpapiBrowserHost::evaluateJavaScript(const std::string &script)
{
    assertMainThread();
    NPVariant retVal;
    NPVariant tmp;

    this->getNPVariant(&tmp, FB::variant(script));

    if (!m_htmlWin)
        throw std::runtime_error("Cannot find HTML window");

    if (this->Evaluate(m_htmlWin->getNPObject(),
                       &tmp.value.stringValue, &retVal)) {
        this->ReleaseVariantValue(&retVal);
        /* Throw away returned variant. NPN_Evaluate supports returning
           stuff from JS, but ActiveX IHTMLWindow2::execScript does not */
        return;
    } else {
        throw script_error("Error executing JavaScript code");
    }
}

FB::variant NpapiBrowserHost::getVariant(const NPVariant *npVar)
{
    FB::variant retVal;
    switch(npVar->type) {
        case NPVariantType_Null:
            retVal = FB::variant_detail::null();
            break;

        case NPVariantType_Bool:
            retVal = npVar->value.boolValue;
            break;

        case NPVariantType_Int32:
            retVal = npVar->value.intValue;
            break;

        case NPVariantType_Double:
            retVal = npVar->value.doubleValue;
            break;

        case NPVariantType_String:
            retVal = std::string(npVar->value.stringValue.UTF8Characters, npVar->value.stringValue.UTF8Length);
            break;

        case NPVariantType_Object:
            retVal = JSObjectPtr(new NPObjectAPI(npVar->value.objectValue, std::dynamic_pointer_cast<NpapiBrowserHost>(shared_from_this())));
            break;

        case NPVariantType_Void:
        default:
            // Do nothing; it's already void =]
            break;
    }

    return retVal;
}

bool NpapiBrowserHost::isSafari() const
{
    // Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_6_6; en-us) AppleWebKit/533.20.25 (KHTML, like Gecko) Version/5.0.4 Safari/533.20.27
    std::string agent(UserAgent());
    return boost::algorithm::contains(agent, "Safari") && !isChrome();
}

bool NpapiBrowserHost::isFirefox() const
{
    // Mozilla/5.0 (Macintosh; Intel Mac OS X 10.6; rv:2.0) Gecko/20100101 Firefox/4.0
    // Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10.6; en-US; rv:1.9.2.15) Gecko/20110303 Firefox/3.6.15
    std::string agent(UserAgent());
    return boost::algorithm::contains(agent, "Firefox");
}

bool NpapiBrowserHost::isChrome() const
{
    // Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_6_6; en-US) AppleWebKit/534.16 (KHTML, like Gecko) Chrome/10.0.648.204 Safari/534.16
    std::string agent(UserAgent());
    return boost::algorithm::contains(agent, "Chrome");
}

void NpapiBrowserHost::getNPVariant(NPVariant *dst, const FB::variant &var)
{
    assertMainThread();

    const NPVariantBuilderMap& builderMap = getNPVariantBuilderMap();
    const std::type_info& type = var.get_type();
    NPVariantBuilderMap::const_iterator it = builderMap.find(&type);

    if (it == builderMap.end()) {
        // unhandled type :(
        return;
    }

    *dst = (it->second)(std::dynamic_pointer_cast<NpapiBrowserHost>(shared_from_this()), var);
}

NPError NpapiBrowserHost::GetURLNotify(const char* url, const char* target, void* notifyData) const
{
    assertMainThread();
    if (NPNFuncs.geturlnotify != nullptr) {
        return NPNFuncs.geturlnotify(m_npp, url, target, notifyData);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::GetURL(const char* url, const char* target) const
{
    assertMainThread();
    if (NPNFuncs.geturl != nullptr) {
        return NPNFuncs.geturl(m_npp, url, target);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::PostURLNotify(const char* url, const char* target, uint32_t len,
                                        const char* buf, NPBool file, void* notifyData) const
{
    assertMainThread();
    if (NPNFuncs.posturlnotify != nullptr) {
        return NPNFuncs.posturlnotify(m_npp, url, target, len, buf, file, notifyData);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::PostURL(const char* url, const char* target, uint32_t len,
                                  const char* buf, NPBool file) const
{
    assertMainThread();
    if (NPNFuncs.posturl != nullptr) {
        return NPNFuncs.posturl(m_npp, url, target, len, buf, file);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::RequestRead(NPStream* stream, NPByteRange* rangeList) const
{
    assertMainThread();
    if (NPNFuncs.requestread != nullptr) {
        return NPNFuncs.requestread(stream, rangeList);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::NewStream(NPMIMEType type, const char* target, NPStream** stream) const
{
    assertMainThread();
    if (NPNFuncs.newstream != nullptr) {
        return NPNFuncs.newstream(m_npp, type, target, stream);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

int32_t NpapiBrowserHost::Write(NPStream* stream, int32_t len, void* buffer) const
{
    assertMainThread();
    if (NPNFuncs.write != nullptr) {
        return NPNFuncs.write(m_npp, stream, len, buffer);
    } else {
        return 0;
    }
}

NPError NpapiBrowserHost::DestroyStream(NPStream* stream, NPReason reason) const
{
    assertMainThread();
    if (NPNFuncs.destroystream != nullptr) {
        return NPNFuncs.destroystream(m_npp, stream, reason);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

void* NpapiBrowserHost::MemAlloc(uint32_t size) const
{
    return module->MemAlloc(size);
}
void NpapiBrowserHost::MemFree(void* ptr) const
{
    module->MemFree(ptr);
}
uint32_t NpapiBrowserHost::MemFlush(uint32_t size) const
{
    return module->MemFlush(size);
}

NPObject *NpapiBrowserHost::RetainObject(NPObject *npobj) const
{
    assertMainThread();
    return module->RetainObject(npobj);
}
void NpapiBrowserHost::ReleaseObject(NPObject *npobj) const
{
    assertMainThread();
    return module->ReleaseObject(npobj);
}
void NpapiBrowserHost::ReleaseVariantValue(NPVariant *variant) const
{
    assertMainThread();
    return module->ReleaseVariantValue(variant);
}

NPIdentifier NpapiBrowserHost::GetStringIdentifier(const NPUTF8 *name) const
{
    assertMainThread();
    return module->GetStringIdentifier(name);
}
void NpapiBrowserHost::GetStringIdentifiers(const NPUTF8 **names, int32_t nameCount, NPIdentifier *identifiers) const
{
    assertMainThread();
    return module->GetStringIdentifiers(names, nameCount, identifiers);
}
NPIdentifier NpapiBrowserHost::GetIntIdentifier(int32_t intid) const
{
    assertMainThread();
    return module->GetIntIdentifier(intid);
}
bool NpapiBrowserHost::IdentifierIsString(NPIdentifier identifier) const
{
    assertMainThread();
    return module->IdentifierIsString(identifier);
}
NPUTF8 *NpapiBrowserHost::UTF8FromIdentifier(NPIdentifier identifier) const
{
    assertMainThread();
    return module->UTF8FromIdentifier(identifier);
}
std::string NpapiBrowserHost::StringFromIdentifier(NPIdentifier identifier) const
{
    assertMainThread();
    return module->StringFromIdentifier(identifier);
}
int32_t NpapiBrowserHost::IntFromIdentifier(NPIdentifier identifier) const
{
    assertMainThread();
    return module->IntFromIdentifier(identifier);
}


void NpapiBrowserHost::SetStatus(const char* message) const
{
    assertMainThread();
    if (NPNFuncs.status != nullptr) {
        NPNFuncs.status(m_npp, message);
    }
}

const char* NpapiBrowserHost::UserAgent() const
{
    assertMainThread();
    if (NPNFuncs.uagent != nullptr) {
        return NPNFuncs.uagent(m_npp);
    } else {
        return nullptr;
    }
}

NPError NpapiBrowserHost::GetValue(NPNVariable variable, void *value) const
{
    assertMainThread();
    if (NPNFuncs.getvalue != nullptr) {
        return NPNFuncs.getvalue(m_npp, variable, value);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::SetValue(NPPVariable variable, void *value) const
{
    assertMainThread();
    if (NPNFuncs.setvalue != nullptr) {
        return NPNFuncs.setvalue(m_npp, variable, value);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

void NpapiBrowserHost::InvalidateRect2(const NPRect& invalidRect) const
{
    NPRect rect = invalidRect;
    return InvalidateRect(&rect);
}

void NpapiBrowserHost::InvalidateRect(NPRect *invalidRect) const
{
    assertMainThread();
    if (NPNFuncs.invalidaterect != nullptr) {
        NPNFuncs.invalidaterect(m_npp, invalidRect);
    }
}

void NpapiBrowserHost::InvalidateRegion(NPRegion invalidRegion) const
{
    assertMainThread();
    if (NPNFuncs.invalidateregion != nullptr) {
        NPNFuncs.invalidateregion(m_npp, invalidRegion);
    }
}

void NpapiBrowserHost::ForceRedraw() const
{
    assertMainThread();
    if (NPNFuncs.forceredraw != nullptr) {
        NPNFuncs.forceredraw(m_npp);
    }
}

void NpapiBrowserHost::PushPopupsEnabledState(NPBool enabled) const
{
    assertMainThread();
    if (NPNFuncs.pushpopupsenabledstate != nullptr) {
        NPNFuncs.pushpopupsenabledstate(m_npp, enabled);
    }
}

void NpapiBrowserHost::PopPopupsEnabledState() const
{
    assertMainThread();
    if (NPNFuncs.poppopupsenabledstate != nullptr) {
        NPNFuncs.poppopupsenabledstate(m_npp);
    }
}

void NpapiBrowserHost::PluginThreadAsyncCall(void (*func) (void *), void *userData) const
{
    if (NPNFuncs.pluginthreadasynccall != nullptr) {
        NPNFuncs.pluginthreadasynccall(m_npp, func, userData);
    }
}

/* npruntime.h definitions */
NPObject *NpapiBrowserHost::CreateObject(NPClass *aClass) const
{
    assertMainThread();
    if (NPNFuncs.createobject != nullptr) {
        return NPNFuncs.createobject(m_npp, aClass);
    } else {
        return nullptr;
    }
}

bool NpapiBrowserHost::Invoke(NPObject *npobj, NPIdentifier methodName, const NPVariant *args,
                              uint32_t argCount, NPVariant *result) const
{
    assertMainThread();
    if (NPNFuncs.invoke != nullptr) {
        return NPNFuncs.invoke(m_npp, npobj, methodName, args, argCount, result);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::InvokeDefault(NPObject *npobj, const NPVariant *args,
                                     uint32_t argCount, NPVariant *result) const
{
    assertMainThread();
    if (NPNFuncs.invokeDefault != nullptr) {
        return NPNFuncs.invokeDefault(m_npp, npobj, args, argCount, result);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::Evaluate(NPObject *npobj, NPString *script,
                                NPVariant *result) const
{
    assertMainThread();
    if (NPNFuncs.evaluate != nullptr) {
        return NPNFuncs.evaluate(m_npp, npobj, script, result);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::GetProperty(NPObject *npobj, NPIdentifier propertyName,
                                   NPVariant *result) const
{
    assertMainThread();
    if (NPNFuncs.getproperty != nullptr) {
        return NPNFuncs.getproperty(m_npp, npobj, propertyName, result);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::SetProperty(NPObject *npobj, NPIdentifier propertyName,
                                   const NPVariant *value) const
{
    assertMainThread();
    if (NPNFuncs.setproperty != nullptr) {
        return NPNFuncs.setproperty(m_npp, npobj, propertyName, value);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::RemoveProperty(NPObject *npobj, NPIdentifier propertyName) const
{
    assertMainThread();
    if (NPNFuncs.removeproperty != nullptr) {
        return NPNFuncs.removeproperty(m_npp, npobj, propertyName);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::HasProperty(NPObject *npobj, NPIdentifier propertyName) const
{
    assertMainThread();
    if (NPNFuncs.hasproperty != nullptr) {
        return NPNFuncs.hasproperty(m_npp, npobj, propertyName);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::HasMethod(NPObject *npobj, NPIdentifier methodName) const
{
    assertMainThread();
    if (NPNFuncs.hasmethod != nullptr) {
        return NPNFuncs.hasmethod(m_npp, npobj, methodName);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::Enumerate(NPObject *npobj, NPIdentifier **identifier,
                                 uint32_t *count) const
{
    assertMainThread();
    if (NPNFuncs.enumerate != nullptr) {
        return NPNFuncs.enumerate(m_npp, npobj, identifier, count);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::Construct(NPObject *npobj, const NPVariant *args,
                                 uint32_t argCount, NPVariant *result) const
{
    assertMainThread();
    if (NPNFuncs.construct != nullptr) {
        return NPNFuncs.construct(m_npp, npobj, args, argCount, result);
    } else {
        return false;
    }
}

void NpapiBrowserHost::SetException(NPObject *npobj, const NPUTF8 *message) const
{
    assertMainThread();
    if (NPNFuncs.setexception != nullptr) {
        NPNFuncs.setexception(npobj, message);
    }
}

int NpapiBrowserHost::ScheduleTimer(int interval, bool repeat, void(*func)(NPP npp, uint32_t timerID)) const
{
    if(NPNFuncs.scheduletimer != nullptr) {
        return NPNFuncs.scheduletimer(m_npp, interval, repeat, func);
    } else {
        return 0;
    }
}

void NpapiBrowserHost::UnscheduleTimer(int timerId)  const
{
    if(NPNFuncs.unscheduletimer != nullptr) {
        NPNFuncs.unscheduletimer(m_npp, timerId);
    }
}

NPError NpapiBrowserHost::GetValueForURL( NPNURLVariable variable, const char *url, char **value, uint32_t *len )
{
    if(NPNFuncs.getvalueforurl != nullptr) {
        return NPNFuncs.getvalueforurl(m_npp, variable, url, value, len);
    } else {
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }
}

NPError NpapiBrowserHost::SetValueForURL( NPNURLVariable variable, const char *url, const char *value, uint32_t len )
{
    if(NPNFuncs.setvalueforurl != nullptr) {
        return NPNFuncs.setvalueforurl(m_npp, variable, url, value, len);
    } else {
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }
}

NPError NpapiBrowserHost::GetAuthenticationInfo( const char *protocol,
                                                            const char *host, int32_t port,
                                                            const char *scheme, const char *realm,
                                                            char **username, uint32_t *ulen,
                                                            char **password, uint32_t *plen )
{
    if(NPNFuncs.getauthenticationinfo != nullptr) {
        return NPNFuncs.getauthenticationinfo(m_npp, protocol, host, port, scheme, realm,
                                              username, ulen, password, plen);
    } else {
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }
}

FB::BrowserStreamPtr NpapiBrowserHost::_createStream( const FB::BrowserStreamRequest& req ) const
{
    assertMainThread();
    std::string url(req.uri.toString());
    NpapiStreamPtr stream( std::make_shared<NpapiStream>( url, req.cache, req.seekable, req.internalBufferSize, std::dynamic_pointer_cast<const NpapiBrowserHost>(shared_from_this()) ) );
    if (req.getEventSink()) {
        stream->AttachObserver( req.getEventSink() );
    }

    NPError err;
    if (req.method == "GET") {
        err = GetURLNotify(url.c_str(), 0, stream.get());
    } else {
        std::stringstream postOutput;
        std::string postdata = req.getPostData();
        std::string postheaders = req.getPostHeaders();
        if (!postheaders.empty()) {
            postOutput << postheaders << "\n\n";
        } else {
            postOutput << "Content-type: application/x-www-form-urlencoded\n";
            postOutput << "Content-Length: " << postdata.length() << "\n\n";
        }
        postOutput << postdata;
        std::string out = postOutput.str();
        err = PostURLNotify( url.c_str(), 0, out.length(), out.c_str(), false, stream.get() );
    }
    if (err == NPERR_NO_ERROR) {
        stream->setCreated();
        StreamCreatedEvent ev(stream.get());
        stream->SendEvent( &ev );
    }
    else
    {
        stream.reset();
    }
    return stream;
}

FB::BrowserStreamPtr NpapiBrowserHost::_createUnsolicitedStream(const FB::BrowserStreamRequest& req) const
{
    std::string url = req.uri.toString();
    FBLOG_TRACE("NpapiBrowserStream", "Creating an unsolicited stream with url: " << url);
    bool cache(false);
    NpapiStreamPtr stream( std::make_shared<NpapiStream>( url, cache, req.seekable, req.internalBufferSize, std::dynamic_pointer_cast<const NpapiBrowserHost>(shared_from_this()) ) );
    // The observer is attached by the caller

    stream->setCreated();
    // we're not waiting for a URLNotify call from this stream
    stream->setNotified();
    StreamCreatedEvent ev(stream.get());
    stream->SendEvent( &ev );
    return stream;
}

NPJavascriptObject* NpapiBrowserHost::getJSAPIWrapper( const FB::JSAPIWeakPtr& api, bool autoRelease/* = false*/ )
{
    assertMainThread(); // This should only be called on the main thread
    typedef std::shared_ptr<FB::ShareableReference<NPJavascriptObject> > SharedNPObjectRef;
    NPJavascriptObject* ret(nullptr);
    FB::JSAPIPtr ptr(api.lock());
    if (!ptr)
        return NPJavascriptObject::NewObject(std::dynamic_pointer_cast<NpapiBrowserHost>(shared_from_this()), api, false);

    NPObjectRefMap::iterator fnd = m_cachedNPObject.find(ptr.get());
    if (fnd != m_cachedNPObject.end()) {
        SharedNPObjectRef ref(fnd->second.lock());
        if (ref) {
            // Fortunately this doesn't have to be threadsafe since this method only gets called
            // from the main thread and the browser access happens on that thread as well!
            ret = ref->getPtr();
            RetainObject(ret);
        } else {
            m_cachedNPObject.erase(fnd);
        }
    }
    if (!ret) {
        ret = NPJavascriptObject::NewObject(std::dynamic_pointer_cast<NpapiBrowserHost>(shared_from_this()), api, autoRelease);
        if (ret)
            m_cachedNPObject[ptr.get()] = ret->getWeakReference();
    }
    return ret;
}

bool NpapiBrowserHost::DetectProxySettings( std::map<std::string, std::string>& settingsMap, std::string URL )
{
    char* retVal;
    uint32_t len;
    NPError err = GetValueForURL(NPNURLVProxy, URL.c_str(), &retVal, &len);
    if (err != NPERR_NO_ERROR) {
        // Only fall back to system proxy detection if NPAPI's API isn't supported on this browser
        if (err == NPERR_INCOMPATIBLE_VERSION_ERROR)
            return FB::BrowserHost::DetectProxySettings(settingsMap, URL);
        else
            return false;
    }
    std::string res(retVal, len);
    MemFree(retVal);

    if (res == "DIRECT") {
        return false;
    } else {
        settingsMap.clear();
        vector<string> params;
        // Chrome returns the whole proxy list string,
        // squashing any space symbols between two consecutive proxies,
        // thus we need semicolon as a separator also:
        split(params, res, is_any_of(" ;"));
        vector<string> host;
        split(host, params[1], is_any_of(":"));
        if (params[0] == "PROXY") {
            FB::URI uri = FB::URI::fromString(URL);
            settingsMap["type"] = uri.protocol;
        } else if(params[0] == "SOCKS" ||
                  params[0] == "SOCKS5") {
            settingsMap["type"] = "socks";
        } else {
            settingsMap["type"] = params[0];
        }
        settingsMap["hostname"] = host[0];
        settingsMap["port"] = host[1];
        return true;
    }
}

void NpapiBrowserHost::Navigate( std::string url, std::string target )
{
    PushPopupsEnabledState(true);
    GetURL(url.c_str(), target.c_str());
    PopPopupsEnabledState();
}

NPObject * NpapiBrowserHost::getPromiseObject() {
    NPObject *window = m_htmlWin->getNPObject();
    NPVariant tmp;
    NPIdentifier idFbDeferred = GetStringIdentifier("FireBreathPromise");
    bool success = Invoke(window, idFbDeferred, nullptr, 0, &tmp);

    if (!success || tmp.type != NPVariantType_Object) {
        // We couldn't find the FireBreathPromise object already in the page
        NPVariant jsStr;
        getNPVariant(&jsStr, BrowserHost::fbPromiseJS);
        if (!Evaluate(window, &jsStr.value.stringValue, &tmp)) {
            throw std::runtime_error("Could not inject Promise library");
        }
        ReleaseVariantValue(&tmp);
        success = Invoke(window, idFbDeferred, nullptr, 0, &tmp);
        if (!success || tmp.type != NPVariantType_Object) {
            throw std::runtime_error("Could not instantiate Promise");
        }
    }
    NPObject* dfd = tmp.value.objectValue;
    RetainObject(dfd);
    ReleaseVariantValue(&tmp);
    return dfd;
}

NPObject* NpapiBrowserHost::makeError(const std::exception& e) {
    FBLOG_ERROR("makeError", "An error occurred: " << e.what());
    
    NPObject *jsHelper = getJSHelper();
    NPVariant param, res;

    NPIdentifier idFn = GetStringIdentifier("makeError");

    getNPVariant(&param, FB::variant(e.what()));
    bool success = Invoke(jsHelper, idFn, &param, 1, &res);
    if (success && res.type == NPVariantType_Object) {
        NPObject* errObj = res.value.objectValue;
        RetainObject(errObj);
        ReleaseVariantValue(&res);
        ReleaseVariantValue(&param);
        return errObj;
    } else if (success) {
        ReleaseVariantValue(&res);
    }
    ReleaseVariantValue(&param);
    return nullptr;
}

NPObject* NpapiBrowserHost::getJSHelper() {
    NPVariant tmp;
    if (!m_jsHelper) {
        NPObject *window = m_htmlWin->getNPObject();
        
        uint32_t ctxId( (uintptr_t)(getContextID()) );
        std::string name = std::string("_FB_HELPERS_") + std::to_string(ctxId);
        
        NPIdentifier idFbObj = GetStringIdentifier(name.c_str());
        bool success = GetProperty(window, idFbObj, &tmp);
        if (success && tmp.type == NPVariantType_Object) {
            m_jsHelper = tmp.value.objectValue;
            RetainObject(m_jsHelper);
        }
        ReleaseVariantValue(&tmp);

        if (!m_jsHelper) {
            // Didn't find it, probably needs to be added to the page
            std::string newjs{ BrowserHost::jsHelperTpl };
            replace_first(newjs, "FireBreathHelperThingy", name);
            NPVariant jsStr;
            getNPVariant(&jsStr, newjs);
            if (!Evaluate(window, &jsStr.value.stringValue, &tmp)) {
                throw std::runtime_error("Could not inject JS Helper into page");
            }
            ReleaseVariantValue(&tmp);
            success = GetProperty(window, idFbObj, &tmp);
            if (!success || tmp.type != NPVariantType_Object) {
                throw std::runtime_error("Could not find injected helper");
            }
            m_jsHelper = tmp.value.objectValue;
            RetainObject(m_jsHelper);
            ReleaseVariantValue(&tmp);
        }
    }
    return m_jsHelper;
}

int NpapiBrowserHost::delayedInvoke(const int delayms, const FB::JSObjectPtr& func, const FB::VariantList& args, std::string fname /*= ""*/) {
    NPObject *jsHelper = getJSHelper();
    NPVariant tmp;

    FB::VariantList argList{ delayms, func, args, fname };
    std::array<NPVariant, 4> npArgs;
    decltype(npArgs.size()) i = 0;
    for (auto v : argList) {
        getNPVariant(&npArgs[i++], v);
    };

    NPIdentifier idAsyncCall = GetStringIdentifier("asyncCall");
    if (Invoke(jsHelper, idAsyncCall, npArgs.data(), npArgs.size(), &tmp)) {
        int res = tmp.value.intValue;
        ReleaseVariantValue(&tmp);
        return res;
    } else {
        throw std::runtime_error("Could not launch async call");
    }
}

NPError NpapiBrowserHost::InitAsyncSurface(NPSize *size, NPImageFormat format, void *initData, NPAsyncSurface *surface) const
{
    if (NPNFuncs.initasyncsurface) {
        return NPNFuncs.initasyncsurface(m_npp, size, format, initData, surface);
    }
    return NPERR_GENERIC_ERROR;
}

NPError NpapiBrowserHost::FinalizeAsyncSurface(NPAsyncSurface *surface) const
{
    if (NPNFuncs.finalizeasyncsurface) {
        return NPNFuncs.finalizeasyncsurface(m_npp, surface);
    }
    return NPERR_GENERIC_ERROR;
}

void NpapiBrowserHost::SetCurrentAsyncSurface(NPAsyncSurface *surface, NPRect *changed) const
{
    if (NPNFuncs.setcurrentasyncsurface) {
        NPNFuncs.setcurrentasyncsurface(m_npp, surface, changed);
    }
}
