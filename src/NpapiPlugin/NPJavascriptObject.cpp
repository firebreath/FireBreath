/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 16, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "NPJavascriptObject.h"
#include "BrowserObjectAPI.h"

using namespace FB::Npapi;

NPJavascriptObject *NPJavascriptObject::NewObject(NpapiBrowserHost *host, FB::JSAPI *api)
{
    NPJavascriptObject *obj = static_cast<NPJavascriptObject *>(host->CreateObject(&NPJavascriptObjectClass));

    obj->setAPI(api, host);
    return obj;
}

NPJavascriptObject::NPJavascriptObject(NPP npp)
    : m_api(NULL), m_browser(NULL), m_valid(true)
{
}

NPJavascriptObject::~NPJavascriptObject(void)
{
}

void NPJavascriptObject::setAPI(FB::JSAPI *api, NpapiBrowserHost *host)
{
    m_api = api;
    m_browser = host;
}

void NPJavascriptObject::Invalidate()
{
    m_valid = false;
    m_api->invalidate();
}

bool NPJavascriptObject::HasMethod(NPIdentifier name)
{
    std::string mName = m_browser->StringFromIdentifier(name);

    if (mName == "addEventListener" || mName == "removeEventListener") {
        return true;
    } else {
        return m_api->HasMethod(mName);
    }
}

bool NPJavascriptObject::callSetEventListener(std::vector<FB::variant> &args, bool add)
{
    if (args.size() < 2 || args.size() > 3
         || args[0].get_type() != typeid(std::string)
         || args[1].get_type() != typeid(JSObject)) {
        throw invalid_arguments();
    }

    std::string evtName = "on" + args[0].convert_cast<std::string>();
    if (add) {
        m_api->registerEventMethod(evtName,
            args[1].convert_cast<JSObject>());
    } else {
        m_api->unregisterEventMethod(evtName,
            args[1].convert_cast<JSObject>());
    }

    return true;
}

bool NPJavascriptObject::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    VOID_TO_NPVARIANT(*result);
    try {
        std::string mName = m_browser->StringFromIdentifier(name);
        std::vector<FB::variant> vArgs;
        for (unsigned int i = 0; i < argCount; i++) {
            vArgs.push_back(m_browser->getVariant(&args[i]));
        }

        // Event Handling
        if (mName == "addEventListener") {
            return callSetEventListener(vArgs, true);
        } else if (mName == "removeEventListener") {
            return callSetEventListener(vArgs, false);
        } else {
            // Default method call
            FB::variant ret = m_api->Invoke(mName, vArgs);
            m_browser->getNPVariant(result, ret);
            return true;
        }
    } catch (const script_error& e) {
        m_browser->SetException(this, e.what());
        return false;
    }
}

bool NPJavascriptObject::InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    return Invoke(m_browser->GetStringIdentifier(""), args, argCount, result);
}

bool NPJavascriptObject::HasProperty(NPIdentifier name)
{
    std::string sName(m_browser->StringFromIdentifier(name));
    // We check for events of that name as well in order to allow setting of an event handler in the
    // old javascript style, i.e. plugin.onload = function() .....;
    return m_api->HasEvent(sName) || m_api->HasProperty(sName);
}

bool NPJavascriptObject::GetProperty(NPIdentifier name, NPVariant *result)
{
    try {
        FB::variant res;
        if (m_browser->IdentifierIsString(name)) {
            std::string sName(m_browser->StringFromIdentifier(name));
            if (m_api->HasEvent(sName)) {
                FB::JSObject tmp(m_api->getDefaultEventMethod(sName));
                if (tmp.ptr() != NULL)
                    res = tmp;
            } else {
                res = m_api->GetProperty(sName);
            }
        } else {
            res = m_api->GetProperty(m_browser->IntFromIdentifier(name));
        }

        m_browser->getNPVariant(result, res);
        return true;
    } catch (const script_error& e) {
        m_browser->SetException(this, e.what());
        return false;
    }
}

bool NPJavascriptObject::SetProperty(NPIdentifier name, const NPVariant *value)
{
    try {
        FB::variant arg = m_browser->getVariant(value);
        if (m_browser->IdentifierIsString(name)) {
            std::string sName(m_browser->StringFromIdentifier(name));
            if (m_api->HasEvent(sName)) {
                if (arg.get_type() != typeid(FB::JSObject)) {
                    throw script_error("Invalid event handler function");
                } else {
                    FB::JSObject tmp(arg.cast<FB::JSObject>());
                    m_api->setDefaultEventMethod(sName, tmp.ptr());
                }
            } else {
                m_api->SetProperty(sName, arg);
            }
        } else {
            m_api->SetProperty(m_browser->IntFromIdentifier(name), arg);
        }
        return true;
    } catch(const script_error& e) {
        m_browser->SetException(this, e.what());
        return false;
    }
}

bool NPJavascriptObject::RemoveProperty(NPIdentifier name)
{
    try {
        if (m_browser->IdentifierIsString(name)) {
            std::string sName(m_browser->StringFromIdentifier(name));
            if (m_api->HasEvent(sName)) {
                m_api->setDefaultEventMethod(sName, NULL);
            }
        }
        return false;
    } catch(const script_error& e) {
        m_browser->SetException(this, e.what());
        return false;
    }
}

bool NPJavascriptObject::Enumeration(NPIdentifier **value, uint32_t *count)
{
    try {
        typedef std::vector<std::string> StringArray;
        StringArray memberList;
        m_api->getMemberNames(memberList);
        *count = memberList.size();
        NPIdentifier *outList(NULL);
        outList = (NPIdentifier*)m_browser->MemAlloc((uint32_t)(sizeof(NPIdentifier) * *count));
        
        for (uint32_t i = 0; i < memberList.size(); i++) {
            outList[i] = m_browser->GetStringIdentifier(memberList.at(i).c_str());
        }
        *value = outList;
        return true;
    } catch (const script_error& e) {
        m_browser->SetException(this, e.what());
        return false;
    }
}

bool NPJavascriptObject::Construct(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    try {
        // TODO: add support for constructing
        return false;
    } catch (const script_error& e) {
        m_browser->SetException(this, e.what());
        return false;
    }
}


/****************************************************************************\
  These are the static functions given to the browser in the NPClass struct.
  You might look at these as the "entry points" for the NPJavascriptObject
\****************************************************************************/

NPObject *NPJavascriptObject::Allocate(NPP npp, NPClass *aClass)
{
    return (NPObject *)new NPJavascriptObject(npp);
}

void NPJavascriptObject::_Deallocate(NPObject *npobj)
{
    delete static_cast<NPJavascriptObject *>(npobj);
}

void NPJavascriptObject::_Invalidate(NPObject *npobj)
{
    NPJavascriptObject *obj = static_cast<NPJavascriptObject *>(npobj);
    obj->Invalidate();
}

bool NPJavascriptObject::_HasMethod(NPObject *npobj, NPIdentifier name)
{
    NPJavascriptObject *obj = static_cast<NPJavascriptObject *>(npobj);
    return obj->HasMethod(name);
}

bool NPJavascriptObject::_Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    NPJavascriptObject *obj = static_cast<NPJavascriptObject *>(npobj);
    return obj->Invoke(name, args, argCount, result);
}

bool NPJavascriptObject::_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    NPJavascriptObject *obj = static_cast<NPJavascriptObject *>(npobj);
    return obj->InvokeDefault(args, argCount, result);
}

bool NPJavascriptObject::_HasProperty(NPObject *npobj, NPIdentifier name)
{
    NPJavascriptObject *obj = static_cast<NPJavascriptObject *>(npobj);
    return obj->HasProperty(name);
}

bool NPJavascriptObject::_GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result)
{
    NPJavascriptObject *obj = static_cast<NPJavascriptObject *>(npobj);
    return obj->GetProperty(name, result);
}

bool NPJavascriptObject::_SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value)
{
    NPJavascriptObject *obj = static_cast<NPJavascriptObject *>(npobj);
    return obj->SetProperty(name, value);
}

bool NPJavascriptObject::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
    NPJavascriptObject *obj = static_cast<NPJavascriptObject *>(npobj);
    return obj->RemoveProperty(name);
}

bool NPJavascriptObject::_Enumeration(NPObject *npobj, NPIdentifier **value, uint32_t *count)
{
    NPJavascriptObject *obj = static_cast<NPJavascriptObject *>(npobj);
    return obj->Enumeration(value, count);
}

bool NPJavascriptObject::_Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    NPJavascriptObject *obj = static_cast<NPJavascriptObject *>(npobj);
    return obj->Construct(args, argCount, result);
}

// This defines the "entry points"; it's how the browser knows how to create the object
// when you call NPN_CreateObject, and how it knows how to call functions on it
NPClass NPJavascriptObject::NPJavascriptObjectClass = {
    NP_CLASS_STRUCT_VERSION_CTOR,
    NPJavascriptObject::Allocate,
    NPJavascriptObject::_Deallocate,
    NPJavascriptObject::_Invalidate,
    NPJavascriptObject::_HasMethod,
    NPJavascriptObject::_Invoke,
    NPJavascriptObject::_InvokeDefault,
    NPJavascriptObject::_HasProperty,
    NPJavascriptObject::_GetProperty,
    NPJavascriptObject::_SetProperty,
    NPJavascriptObject::_RemoveProperty,
    NPJavascriptObject::_Enumeration,
    NPJavascriptObject::_Construct
};