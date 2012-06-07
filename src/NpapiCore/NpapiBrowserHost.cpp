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
#include "variant_list.h"

#include "NpapiStream.h"
#include "NpapiBrowserHost.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include "NPVariantUtil.h"
#include "URI.h"

using namespace FB::Npapi;

using boost::algorithm::split;
using boost::algorithm::is_any_of;
using boost::algorithm::istarts_with;
using std::vector;
using std::string;
using std::map;

namespace
{
    struct MethodCallReq
    {
        //FB::variant
    };

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

        return tdm;
    }

    const NPVariantBuilderMap& getNPVariantBuilderMap()
    {
        static const NPVariantBuilderMap tdm = makeNPVariantBuilderMap();
        return tdm;
    }
}

NpapiBrowserHost::NpapiBrowserHost(NpapiPluginModule *module, NPP npp)
    : module(module), m_npp(npp)
{
    assert(module != NULL);
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

    NPObject *window(NULL);
    NPObject *element(NULL);
    try {
        GetValue(NPNVWindowNPObject, (void**)&window);
        GetValue(NPNVPluginElementNPObject, (void**)&element);

        m_htmlWin = NPObjectAPIPtr(new FB::Npapi::NPObjectAPI(window, ptr_cast<NpapiBrowserHost>(shared_from_this())));
        m_htmlElement = NPObjectAPIPtr(new FB::Npapi::NPObjectAPI(element, ptr_cast<NpapiBrowserHost>(shared_from_this())));
        ReleaseObject(window);
        ReleaseObject(element);
    } catch (...) {
        if (window && !m_htmlWin)
            ReleaseObject(window);
        if (element && !m_htmlElement)
            ReleaseObject(element);
    }
    if (m_htmlWin) {
        m_htmlDoc = ptr_cast<NPObjectAPI>(m_htmlWin->GetProperty("document").cast<FB::JSObjectPtr>());
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

FB::DOM::ElementPtr FB::Npapi::NpapiBrowserHost::getDOMElement()
{
    if (!m_htmlElement)
        throw std::runtime_error("Cannot find HTML window");

    return FB::DOM::Element::create(m_htmlElement);
}

void FB::Npapi::NpapiBrowserHost::deferred_release( NPObject* obj )
{
    m_deferredObjects.push(obj);
    if (isMainThread()) {
        DoDeferredRelease();
    }
}

bool isExpired(std::pair<void*, FB::Npapi::NPObjectWeakRef> cur) {
    return cur.second.expired();
}

void FB::Npapi::NpapiBrowserHost::DoDeferredRelease() const
{
    assertMainThread();
    NPObject* cur(NULL);
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
            retVal = JSObjectPtr(new NPObjectAPI(npVar->value.objectValue, ptr_cast<NpapiBrowserHost>(shared_from_this())));
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

    *dst = (it->second)(FB::ptr_cast<NpapiBrowserHost>(shared_from_this()), var);
}

NPError NpapiBrowserHost::GetURLNotify(const char* url, const char* target, void* notifyData) const
{
    assertMainThread();
    if (NPNFuncs.geturlnotify != NULL) {
        return NPNFuncs.geturlnotify(m_npp, url, target, notifyData);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::GetURL(const char* url, const char* target) const
{
    assertMainThread();
    if (NPNFuncs.geturl != NULL) {
        return NPNFuncs.geturl(m_npp, url, target);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::PostURLNotify(const char* url, const char* target, uint32_t len,
                                        const char* buf, NPBool file, void* notifyData) const
{
    assertMainThread();
    if (NPNFuncs.posturlnotify != NULL) {
        return NPNFuncs.posturlnotify(m_npp, url, target, len, buf, file, notifyData);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::PostURL(const char* url, const char* target, uint32_t len,
                                  const char* buf, NPBool file) const
{
    assertMainThread();
    if (NPNFuncs.posturl != NULL) {
        return NPNFuncs.posturl(m_npp, url, target, len, buf, file);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::RequestRead(NPStream* stream, NPByteRange* rangeList) const
{
    assertMainThread();
    if (NPNFuncs.requestread != NULL) {
        return NPNFuncs.requestread(stream, rangeList);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::NewStream(NPMIMEType type, const char* target, NPStream** stream) const
{
    assertMainThread();
    if (NPNFuncs.newstream != NULL) {
        return NPNFuncs.newstream(m_npp, type, target, stream);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

int32_t NpapiBrowserHost::Write(NPStream* stream, int32_t len, void* buffer) const
{
    assertMainThread();
    if (NPNFuncs.write != NULL) {
        return NPNFuncs.write(m_npp, stream, len, buffer);
    } else {
        return 0;
    }
}

NPError NpapiBrowserHost::DestroyStream(NPStream* stream, NPReason reason) const
{
    assertMainThread();
    if (NPNFuncs.destroystream != NULL) {
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
    if (NPNFuncs.status != NULL) {
        NPNFuncs.status(m_npp, message);
    }
}

const char* NpapiBrowserHost::UserAgent() const
{
    assertMainThread();
    if (NPNFuncs.uagent != NULL) {
        return NPNFuncs.uagent(m_npp);
    } else {
        return NULL;
    }
}

NPError NpapiBrowserHost::GetValue(NPNVariable variable, void *value) const
{
    assertMainThread();
    if (NPNFuncs.getvalue != NULL) {
        return NPNFuncs.getvalue(m_npp, variable, value);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::SetValue(NPPVariable variable, void *value) const
{
    assertMainThread();
    if (NPNFuncs.setvalue != NULL) {
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
    if (NPNFuncs.invalidaterect != NULL) {
        NPNFuncs.invalidaterect(m_npp, invalidRect);
    }
}

void NpapiBrowserHost::InvalidateRegion(NPRegion invalidRegion) const
{
    assertMainThread();
    if (NPNFuncs.invalidateregion != NULL) {
        NPNFuncs.invalidateregion(m_npp, invalidRegion);
    }
}

void NpapiBrowserHost::ForceRedraw() const
{
    assertMainThread();
    if (NPNFuncs.forceredraw != NULL) {
        NPNFuncs.forceredraw(m_npp);
    }
}

void NpapiBrowserHost::PushPopupsEnabledState(NPBool enabled) const
{
    assertMainThread();
    if (NPNFuncs.pushpopupsenabledstate != NULL) {
        NPNFuncs.pushpopupsenabledstate(m_npp, enabled);
    }
}

void NpapiBrowserHost::PopPopupsEnabledState() const
{
    assertMainThread();
    if (NPNFuncs.poppopupsenabledstate != NULL) {
        NPNFuncs.poppopupsenabledstate(m_npp);
    }
}

void NpapiBrowserHost::PluginThreadAsyncCall(void (*func) (void *), void *userData) const
{
    if (NPNFuncs.pluginthreadasynccall != NULL) {
        NPNFuncs.pluginthreadasynccall(m_npp, func, userData);
    }
}

/* npruntime.h definitions */
NPObject *NpapiBrowserHost::CreateObject(NPClass *aClass) const
{
    assertMainThread();
    if (NPNFuncs.createobject != NULL) {
        return NPNFuncs.createobject(m_npp, aClass);
    } else {
        return NULL;
    }
}

bool NpapiBrowserHost::Invoke(NPObject *npobj, NPIdentifier methodName, const NPVariant *args,
                              uint32_t argCount, NPVariant *result) const
{
    assertMainThread();
    if (NPNFuncs.invoke != NULL) {
        return NPNFuncs.invoke(m_npp, npobj, methodName, args, argCount, result);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::InvokeDefault(NPObject *npobj, const NPVariant *args,
                                     uint32_t argCount, NPVariant *result) const
{
    assertMainThread();
    if (NPNFuncs.invokeDefault != NULL) {
        return NPNFuncs.invokeDefault(m_npp, npobj, args, argCount, result);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::Evaluate(NPObject *npobj, NPString *script,
                                NPVariant *result) const
{
    assertMainThread();
    if (NPNFuncs.evaluate != NULL) {
        return NPNFuncs.evaluate(m_npp, npobj, script, result);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::GetProperty(NPObject *npobj, NPIdentifier propertyName,
                                   NPVariant *result) const
{
    assertMainThread();
    if (NPNFuncs.getproperty != NULL) {
        return NPNFuncs.getproperty(m_npp, npobj, propertyName, result);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::SetProperty(NPObject *npobj, NPIdentifier propertyName,
                                   const NPVariant *value) const
{
    assertMainThread();
    if (NPNFuncs.setproperty != NULL) {
        return NPNFuncs.setproperty(m_npp, npobj, propertyName, value);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::RemoveProperty(NPObject *npobj, NPIdentifier propertyName) const
{
    assertMainThread();
    if (NPNFuncs.removeproperty != NULL) {
        return NPNFuncs.removeproperty(m_npp, npobj, propertyName);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::HasProperty(NPObject *npobj, NPIdentifier propertyName) const
{
    assertMainThread();
    if (NPNFuncs.hasproperty != NULL) {
        return NPNFuncs.hasproperty(m_npp, npobj, propertyName);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::HasMethod(NPObject *npobj, NPIdentifier methodName) const
{
    assertMainThread();
    if (NPNFuncs.hasmethod != NULL) {
        return NPNFuncs.hasmethod(m_npp, npobj, methodName);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::Enumerate(NPObject *npobj, NPIdentifier **identifier,
                                 uint32_t *count) const
{
    assertMainThread();
    if (NPNFuncs.enumerate != NULL) {
        return NPNFuncs.enumerate(m_npp, npobj, identifier, count);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::Construct(NPObject *npobj, const NPVariant *args,
                                 uint32_t argCount, NPVariant *result) const
{
    assertMainThread();
    if (NPNFuncs.construct != NULL) {
        return NPNFuncs.construct(m_npp, npobj, args, argCount, result);
    } else {
        return false;
    }
}

void NpapiBrowserHost::SetException(NPObject *npobj, const NPUTF8 *message) const
{
    assertMainThread();
    if (NPNFuncs.setexception != NULL) {
        NPNFuncs.setexception(npobj, message);
    }
}

int NpapiBrowserHost::ScheduleTimer(int interval, bool repeat, void(*func)(NPP npp, uint32_t timerID)) const
{
    if(NPNFuncs.scheduletimer != NULL) {
        return NPNFuncs.scheduletimer(m_npp, interval, repeat, func);
    } else {
        return 0;
    }
}

void NpapiBrowserHost::UnscheduleTimer(int timerId)  const
{
    if(NPNFuncs.unscheduletimer != NULL) {
        NPNFuncs.unscheduletimer(m_npp, timerId);
    }
}

NPError FB::Npapi::NpapiBrowserHost::GetValueForURL( NPNURLVariable variable, const char *url, char **value, uint32_t *len )
{
    if(NPNFuncs.getvalueforurl != NULL) {
        return NPNFuncs.getvalueforurl(m_npp, variable, url, value, len);
    } else {
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }
}

NPError FB::Npapi::NpapiBrowserHost::SetValueForURL( NPNURLVariable variable, const char *url, const char *value, uint32_t len )
{
    if(NPNFuncs.setvalueforurl != NULL) {
        return NPNFuncs.setvalueforurl(m_npp, variable, url, value, len);
    } else {
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }
}

NPError FB::Npapi::NpapiBrowserHost::GetAuthenticationInfo( const char *protocol,
                                                            const char *host, int32_t port,
                                                            const char *scheme, const char *realm,
                                                            char **username, uint32_t *ulen,
                                                            char **password, uint32_t *plen )
{
    if(NPNFuncs.getauthenticationinfo != NULL) {
        return NPNFuncs.getauthenticationinfo(m_npp, protocol, host, port, scheme, realm,
                                              username, ulen, password, plen);
    } else {
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }
}

FB::BrowserStreamPtr NpapiBrowserHost::_createStream(const std::string& url, const FB::PluginEventSinkPtr& callback,
                                    bool cache, bool seekable, size_t internalBufferSize ) const
{
    NpapiStreamPtr stream( boost::make_shared<NpapiStream>( url, cache, seekable, internalBufferSize, FB::ptr_cast<const NpapiBrowserHost>(shared_from_this()) ) );
    stream->AttachObserver( callback );

    // always use target = 0 for now
    if ( GetURLNotify( url.c_str(), 0, stream.get() ) == NPERR_NO_ERROR )
    {
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

FB::BrowserStreamPtr NpapiBrowserHost::_createPostStream(const std::string& url, const FB::PluginEventSinkPtr& callback,
                                    const std::string& postdata, bool cache, bool seekable, size_t internalBufferSize ) const
{
    NpapiStreamPtr stream( boost::make_shared<NpapiStream>( url, cache, seekable, internalBufferSize, FB::ptr_cast<const NpapiBrowserHost>(shared_from_this()) ) );
    stream->AttachObserver( callback );

    // Add custom headers before data to post!
    std::stringstream headers;
    headers << "Content-type: application/x-www-form-urlencoded\n";
    headers << "Content-Length: " << postdata.length() << "\n\n";
    headers << postdata;

    // always use target = 0 for now
    if ( PostURLNotify( url.c_str(), 0, headers.str().length(), headers.str().c_str(), false, stream.get() ) == NPERR_NO_ERROR )
    {
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
NPJavascriptObject* FB::Npapi::NpapiBrowserHost::getJSAPIWrapper( const FB::JSAPIWeakPtr& api, bool autoRelease/* = false*/ )
{
    assertMainThread(); // This should only be called on the main thread
    typedef boost::shared_ptr<FB::ShareableReference<NPJavascriptObject> > SharedNPObjectRef;
    NPJavascriptObject* ret(NULL);
    FB::JSAPIPtr ptr(api.lock());
    if (!ptr)
        return NPJavascriptObject::NewObject(FB::ptr_cast<NpapiBrowserHost>(shared_from_this()), api, false);

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
        ret = NPJavascriptObject::NewObject(FB::ptr_cast<NpapiBrowserHost>(shared_from_this()), api, autoRelease);
        if (ret)
            m_cachedNPObject[ptr.get()] = ret->getWeakReference();
    }
    return ret;
}

bool FB::Npapi::NpapiBrowserHost::DetectProxySettings( std::map<std::string, std::string>& settingsMap, const std::string& URL )
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
        split(params, res, is_any_of(" "));
        vector<string> host;
        split(host, params[1], is_any_of(":"));
        if (params[0] == "PROXY") {
            FB::URI uri = FB::URI::fromString(URL);
            settingsMap["type"] = uri.protocol;
        } else if(params[0] == "SOCKS") {
            settingsMap["type"] = "socks";
        } else {
            settingsMap["type"] = params[0];
        }
        settingsMap["hostname"] = host[0];
        settingsMap["port"] = host[1];
        return true;
    }
}

void FB::Npapi::NpapiBrowserHost::Navigate( const std::string& url, const std::string& target )
{
    PushPopupsEnabledState(true);
    GetURL(url.c_str(), target.c_str());
    PopPopupsEnabledState();
}
