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

#include <memory.h>
#include <boost/config.hpp>

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

using namespace FB::Npapi;

struct MethodCallReq
{
    //FB::variant
};

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

void NpapiBrowserHost::ScheduleAsyncCall(void (*func)(void *), void *userData)
{
    PluginThreadAsyncCall(func, userData);
}

void NpapiBrowserHost::setBrowserFuncs(NPNetscapeFuncs *pFuncs)
{
    copyNPBrowserFuncs(&NPNFuncs, pFuncs, m_npp);

    NPObject *window(NULL);
    GetValue(NPNVWindowNPObject, (void**)&window);

    m_htmlWin = NPObjectAPIPtr(new FB::Npapi::NPObjectAPI(window, ptr_cast<NpapiBrowserHost>(shared_ptr())));
    ReleaseObject(window);
    if (m_htmlWin) {
        m_htmlDoc = ptr_cast<NPObjectAPI>(m_htmlWin->GetProperty("document").cast<FB::JSObjectPtr>());
    }
}

FB::DOM::DocumentPtr NpapiBrowserHost::getDOMDocument()
{
    if (!m_htmlDoc)
        throw std::runtime_error("Cannot find HTML document");

    return FB::DOM::DocumentPtr(new FB::DOM::Document(m_htmlDoc));
}

FB::DOM::WindowPtr NpapiBrowserHost::getDOMWindow()
{
    if (!m_htmlWin)
        throw std::runtime_error("Cannot find HTML window");

    return FB::DOM::WindowPtr(new FB::DOM::Window(m_htmlWin));
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
            retVal = JSObjectPtr(new NPObjectAPI(npVar->value.objectValue, ptr_cast<NpapiBrowserHost>(shared_ptr())));
            break;

        case NPVariantType_Void:
        default:
            // Do nothing; it's already void =]
            break;
    }

    return retVal;
}

void NpapiBrowserHost::getNPVariant(NPVariant *dst, const FB::variant &var)
{
    assertMainThread();
    if (var.get_type() == typeid(FB::Npapi::NpapiNull)) {
        dst->type = NPVariantType_Null;

    } else if (var.get_type() == typeid(int)        
        || var.get_type() == typeid(short)
        || var.get_type() == typeid(char)
        || var.get_type() == typeid(unsigned short)
        || var.get_type() == typeid(unsigned char)) {
        // Integer type
        dst->type = NPVariantType_Int32;
        dst->value.intValue = var.convert_cast<int32_t>();

    } else if (var.get_type() == typeid(double)
        || var.get_type() == typeid(float)
        || var.get_type() == typeid(unsigned int)
        || var.get_type() == typeid(long)
        || var.get_type() == typeid(unsigned long)
#if !defined(BOOST_NO_LONG_LONG)
        || var.get_type() == typeid(long long)
        || var.get_type() == typeid(unsigned long long)
#endif
        ) {
        // Arithmetic types whose range doesn't fit into int32 
        // (ignoring LLP64 vs. other data models, refactoring will be in 1.4)
        dst->type = NPVariantType_Double;
        dst->value.doubleValue = var.convert_cast<double>();

    } else if (var.get_type() == typeid(bool)) {
        dst->type = NPVariantType_Bool;
        dst->value.boolValue = var.convert_cast<bool>();

    } else if (var.get_type() == typeid(std::string)) {
        std::string str = var.convert_cast<std::string>();
        char *outStr = (char*)this->MemAlloc(str.size() + 1);
        memcpy(outStr, str.c_str(), str.size() + 1);
        dst->type = NPVariantType_String;
        dst->value.stringValue.UTF8Characters = outStr;
        dst->value.stringValue.UTF8Length = str.size();

    } else if (var.get_type() == typeid(std::wstring)) {
        // This is not a typo; the std::string gets the UTF8 representation
        // and we pass that back to the browser
        std::string str = var.convert_cast<std::string>();
        char *outStr = (char*)this->MemAlloc(str.size() + 1);
        memcpy(outStr, str.c_str(), str.size() + 1);
        dst->type = NPVariantType_String;
        dst->value.stringValue.UTF8Characters = outStr;
        dst->value.stringValue.UTF8Length = str.size();

    } else if (var.get_type() == typeid(FB::VariantList)) {
        FB::JSObjectPtr outArr = this->getDOMWindow()->createArray();
        FB::VariantList inArr = var.cast<FB::VariantList>();
        for (FB::VariantList::iterator it = inArr.begin(); it != inArr.end(); it++) {
            outArr->Invoke("push", variant_list_of(*it));
        }
        NPObjectAPIPtr api = ptr_cast<NPObjectAPI>(outArr);
        if (api) {
            dst->type = NPVariantType_Object;
            dst->value.objectValue = api->getNPObject();
            this->RetainObject(dst->value.objectValue);
        }

    } else if (var.get_type() == typeid(FB::VariantMap)) {
        FB::JSObjectPtr out = this->getDOMWindow()->createMap();
        FB::VariantMap inMap = var.cast<FB::VariantMap>();
        for (FB::VariantMap::iterator it = inMap.begin(); it != inMap.end(); it++) {
            out->SetProperty(it->first, it->second);
        }
        NPObjectAPIPtr api = ptr_cast<NPObjectAPI>(out);
        if (api) {
            dst->type = NPVariantType_Object;
            dst->value.objectValue = api->getNPObject();
            this->RetainObject(dst->value.objectValue);
        }

    } else if (var.get_type() == typeid(FB::JSAPIPtr)) {
        NPObject *outObj = NULL;
        FB::JSAPIPtr obj = var.cast<FB::JSAPIPtr>();
        NPObjectAPIPtr tmpObj = ptr_cast<NPObjectAPI>(obj);

        if (obj) {
            if (tmpObj == NULL) {
                outObj = NPJavascriptObject::NewObject(ptr_cast<NpapiBrowserHost>(shared_ptr()), obj);
            } else {
                outObj = tmpObj->getNPObject();
                this->RetainObject(outObj);
            }
            dst->type = NPVariantType_Object;
            dst->value.objectValue = outObj;
        } else {
            dst->type = NPVariantType_Null;
        }
    } else if (var.get_type() == typeid(FB::JSObjectPtr)) {
        NPObject *outObj = NULL;
        FB::JSObjectPtr obj = var.cast<JSObjectPtr>();
        NPObjectAPIPtr tmpObj = ptr_cast<NPObjectAPI>(obj);

        if (obj) {
            if (tmpObj == NULL) {
                outObj = NPJavascriptObject::NewObject(ptr_cast<NpapiBrowserHost>(shared_ptr()), obj);
            } else {
                outObj = tmpObj->getNPObject();
                this->RetainObject(outObj);
            }

            dst->type = NPVariantType_Object;
            dst->value.objectValue = outObj;
        } else {
            dst->type = NPVariantType_Null;
        }
    }
    // TODO: implement object types
}

NPError NpapiBrowserHost::GetURLNotify(const char* url, const char* target, void* notifyData)
{
    assertMainThread();
    if (NPNFuncs.geturlnotify != NULL) {
        return NPNFuncs.geturlnotify(m_npp, url, target, notifyData);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::GetURL(const char* url, const char* target)
{
    assertMainThread();
    if (NPNFuncs.geturl != NULL) {
        return NPNFuncs.geturl(m_npp, url, target);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::PostURLNotify(const char* url, const char* target, uint32_t len,
                                        const char* buf, NPBool file, void* notifyData)
{
    assertMainThread();
    if (NPNFuncs.posturlnotify != NULL) {
        return NPNFuncs.posturlnotify(m_npp, url, target, len, buf, file, notifyData);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::PostURL(const char* url, const char* target, uint32_t len,
                                  const char* buf, NPBool file)
{
    assertMainThread();
    if (NPNFuncs.posturl != NULL) {
        return NPNFuncs.posturl(m_npp, url, target, len, buf, file);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::RequestRead(NPStream* stream, NPByteRange* rangeList)
{
    assertMainThread();
    if (NPNFuncs.requestread != NULL) {
        return NPNFuncs.requestread(stream, rangeList);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::NewStream(NPMIMEType type, const char* target, NPStream** stream)
{
    assertMainThread();
    if (NPNFuncs.newstream != NULL) {
        return NPNFuncs.newstream(m_npp, type, target, stream);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

int32_t NpapiBrowserHost::Write(NPStream* stream, int32_t len, void* buffer)
{
    assertMainThread();
    if (NPNFuncs.write != NULL) {
        return NPNFuncs.write(m_npp, stream, len, buffer);
    } else {
        return 0;
    }
}

NPError NpapiBrowserHost::DestroyStream(NPStream* stream, NPReason reason)
{
    assertMainThread();
    if (NPNFuncs.destroystream != NULL) {
        return NPNFuncs.destroystream(m_npp, stream, reason);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

void* NpapiBrowserHost::MemAlloc(uint32_t size)
{
    return module->MemAlloc(size);
}
void NpapiBrowserHost::MemFree(void* ptr)
{
    module->MemFree(ptr);
}
uint32_t NpapiBrowserHost::MemFlush(uint32_t size)
{
    return module->MemFlush(size);
}

NPObject *NpapiBrowserHost::RetainObject(NPObject *npobj)
{
    assertMainThread();
    return module->RetainObject(npobj);
}
void NpapiBrowserHost::ReleaseObject(NPObject *npobj)
{
    assertMainThread();
    return module->ReleaseObject(npobj);
}
void NpapiBrowserHost::ReleaseVariantValue(NPVariant *variant)
{
    assertMainThread();
    return module->ReleaseVariantValue(variant);
}

NPIdentifier NpapiBrowserHost::GetStringIdentifier(const NPUTF8 *name)
{
    assertMainThread();
    return module->GetStringIdentifier(name);
}
void NpapiBrowserHost::GetStringIdentifiers(const NPUTF8 **names, int32_t nameCount, NPIdentifier *identifiers)
{
    assertMainThread();
    return module->GetStringIdentifiers(names, nameCount, identifiers);
}
NPIdentifier NpapiBrowserHost::GetIntIdentifier(int32_t intid)
{
    assertMainThread();
    return module->GetIntIdentifier(intid);
}
bool NpapiBrowserHost::IdentifierIsString(NPIdentifier identifier)
{
    assertMainThread();
    return module->IdentifierIsString(identifier);
}
NPUTF8 *NpapiBrowserHost::UTF8FromIdentifier(NPIdentifier identifier)
{
    assertMainThread();
    return module->UTF8FromIdentifier(identifier);
}
std::string NpapiBrowserHost::StringFromIdentifier(NPIdentifier identifier)
{
    assertMainThread();
    return module->StringFromIdentifier(identifier);
}
int32_t NpapiBrowserHost::IntFromIdentifier(NPIdentifier identifier)
{
    assertMainThread();
    return module->IntFromIdentifier(identifier);
}


void NpapiBrowserHost::SetStatus(const char* message)
{
    assertMainThread();
    if (NPNFuncs.status != NULL) {
        NPNFuncs.status(m_npp, message);
    }
}

const char* NpapiBrowserHost::UserAgent()
{
    assertMainThread();
    if (NPNFuncs.uagent != NULL) {
        return NPNFuncs.uagent(m_npp);
    } else {
        return NULL;
    }
}

NPError NpapiBrowserHost::GetValue(NPNVariable variable, void *value)
{
    assertMainThread();
    if (NPNFuncs.getvalue != NULL) {
        return NPNFuncs.getvalue(m_npp, variable, value);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

NPError NpapiBrowserHost::SetValue(NPPVariable variable, void *value)
{
    assertMainThread();
    if (NPNFuncs.setvalue != NULL) {
        return NPNFuncs.setvalue(m_npp, variable, value);
    } else {
        return NPERR_GENERIC_ERROR;
    }
}

void NpapiBrowserHost::InvalidateRect(NPRect *invalidRect)
{
    assertMainThread();
    if (NPNFuncs.invalidaterect != NULL) {
        NPNFuncs.invalidaterect(m_npp, invalidRect);
    }
}

void NpapiBrowserHost::InvalidateRegion(NPRegion invalidRegion)
{
    assertMainThread();
    if (NPNFuncs.invalidateregion != NULL) {
        NPNFuncs.invalidateregion(m_npp, invalidRegion);
    }
}

void NpapiBrowserHost::ForceRedraw()
{
    assertMainThread();
    if (NPNFuncs.forceredraw != NULL) {
        NPNFuncs.forceredraw(m_npp);
    }
}

void NpapiBrowserHost::PushPopupsEnabledState(NPBool enabled)
{
    assertMainThread();
    if (NPNFuncs.pushpopupsenabledstate != NULL) {
        NPNFuncs.pushpopupsenabledstate(m_npp, enabled);
    }
}

void NpapiBrowserHost::PopPopupsEnabledState()
{
    assertMainThread();
    if (NPNFuncs.poppopupsenabledstate != NULL) {
        NPNFuncs.poppopupsenabledstate(m_npp);
    }
}

void NpapiBrowserHost::PluginThreadAsyncCall(void (*func) (void *), void *userData)
{
    if (NPNFuncs.pluginthreadasynccall != NULL) {
        NPNFuncs.pluginthreadasynccall(m_npp, func, userData);
    }
}

/* npruntime.h definitions */
NPObject *NpapiBrowserHost::CreateObject(NPClass *aClass)
{
    assertMainThread();
    if (NPNFuncs.createobject != NULL) {
        return NPNFuncs.createobject(m_npp, aClass);
    } else {
        return NULL;
    }
}

bool NpapiBrowserHost::Invoke(NPObject *npobj, NPIdentifier methodName, const NPVariant *args,
                              uint32_t argCount, NPVariant *result)
{
    assertMainThread();
    if (NPNFuncs.invoke != NULL) {
        return NPNFuncs.invoke(m_npp, npobj, methodName, args, argCount, result);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::InvokeDefault(NPObject *npobj, const NPVariant *args,
                                     uint32_t argCount, NPVariant *result)
{
    assertMainThread();
    if (NPNFuncs.invokeDefault != NULL) {
        return NPNFuncs.invokeDefault(m_npp, npobj, args, argCount, result);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::Evaluate(NPObject *npobj, NPString *script,
                                NPVariant *result)
{
    assertMainThread();
    if (NPNFuncs.evaluate != NULL) {
        return NPNFuncs.evaluate(m_npp, npobj, script, result);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::GetProperty(NPObject *npobj, NPIdentifier propertyName,
                                   NPVariant *result)
{
    assertMainThread();
    if (NPNFuncs.getproperty != NULL) {
        return NPNFuncs.getproperty(m_npp, npobj, propertyName, result);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::SetProperty(NPObject *npobj, NPIdentifier propertyName,
                                   const NPVariant *value)
{
    assertMainThread();
    if (NPNFuncs.setproperty != NULL) {
        return NPNFuncs.setproperty(m_npp, npobj, propertyName, value);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::RemoveProperty(NPObject *npobj, NPIdentifier propertyName)
{
    assertMainThread();
    if (NPNFuncs.removeproperty != NULL) {
        return NPNFuncs.removeproperty(m_npp, npobj, propertyName);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::HasProperty(NPObject *npobj, NPIdentifier propertyName)
{
    assertMainThread();
    if (NPNFuncs.hasproperty != NULL) {
        return NPNFuncs.hasproperty(m_npp, npobj, propertyName);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::HasMethod(NPObject *npobj, NPIdentifier methodName)
{
    assertMainThread();
    if (NPNFuncs.hasmethod != NULL) {
        return NPNFuncs.hasmethod(m_npp, npobj, methodName);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::Enumerate(NPObject *npobj, NPIdentifier **identifier,
                                 uint32_t *count)
{
    assertMainThread();
    if (NPNFuncs.enumerate != NULL) {
        return NPNFuncs.enumerate(m_npp, npobj, identifier, count);
    } else {
        return false;
    }
}

bool NpapiBrowserHost::Construct(NPObject *npobj, const NPVariant *args,
                                 uint32_t argCount, NPVariant *result)
{
    assertMainThread();
    if (NPNFuncs.construct != NULL) {
        return NPNFuncs.construct(m_npp, npobj, args, argCount, result);
    } else {
        return false;
    }
}

void NpapiBrowserHost::SetException(NPObject *npobj, const NPUTF8 *message)
{
    assertMainThread();
    if (NPNFuncs.setexception != NULL) {
        NPNFuncs.setexception(npobj, message);
    }
}

int NpapiBrowserHost::ScheduleTimer(int interval, bool repeat, void(*func)(NPP npp, uint32_t timerID))
{
    if(NPNFuncs.scheduletimer != NULL) {
        return NPNFuncs.scheduletimer(m_npp, interval, repeat, func);
    } else {
        return 0;
    }
}

void NpapiBrowserHost::UnscheduleTimer(int timerId) 
{
    if(NPNFuncs.unscheduletimer != NULL) {
        NPNFuncs.unscheduletimer(m_npp, timerId);
    }
}

FB::BrowserStream* NpapiBrowserHost::createStream(const std::string& url, FB::PluginEventSink* callback, 
                                    bool cache, bool seekable, size_t internalBufferSize )
{
    assertMainThread();
    std::auto_ptr<NpapiStream> stream( new NpapiStream( url, cache, seekable, internalBufferSize, this ) );
    stream->AttachObserver( callback );

    // always use target = 0 for now
    if ( GetURLNotify( url.c_str(), 0, stream.get() ) == NPERR_NO_ERROR )
    {
        StreamCreatedEvent ev(stream.get());
        stream->SendEvent( &ev );
        return stream.release();
    }
    else
    {
        stream.reset();
        return 0;
    }
}
