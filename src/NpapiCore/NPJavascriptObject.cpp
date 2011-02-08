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

#include <typeinfo>
#include "JSObject.h"

#include "NPJavascriptObject.h"

using namespace FB::Npapi;

NPJavascriptObject *NPJavascriptObject::NewObject(NpapiBrowserHostPtr host, FB::JSAPIWeakPtr api)
{
    NPJavascriptObject *obj = static_cast<NPJavascriptObject *>(host->CreateObject(&NPJavascriptObjectClass));

    obj->setAPI(api, host);
    return obj;
}

bool NPJavascriptObject::isNPJavaScriptObject(const NPObject* const npo)
{
    return npo->_class == &NPJavascriptObjectClass;
}

NPJavascriptObject::NPJavascriptObject(NPP npp)
    : m_valid(true), m_addEventFunc(boost::make_shared<NPO_addEventListener>(this)),
    m_removeEventFunc(boost::make_shared<NPO_removeEventListener>(this))
{
}

NPJavascriptObject::~NPJavascriptObject(void)
{
}

void NPJavascriptObject::setAPI(FB::JSAPIWeakPtr api, NpapiBrowserHostPtr host)
{
    m_api = api;
    m_browser = host;
}

FB::JSAPIPtr NPJavascriptObject::getAPI() const 
{
    FB::JSAPIPtr ptr(m_api.lock());
    if (!ptr)
        throw std::bad_cast();
    return ptr;
}

void NPJavascriptObject::Invalidate()
{
    m_valid = false;
    try {
        getAPI()->invalidate();
    } catch(const std::bad_cast&) {
    }
}

bool NPJavascriptObject::HasMethod(NPIdentifier name)
{
    try {
        std::string mName = m_browser->StringFromIdentifier(name);
        if (mName == "toString") return true;
        return !getAPI()->HasMethodObject(mName) && getAPI()->HasMethod(mName);
    } catch (const std::bad_cast&) {
        return false; // invalid object
    } catch (const script_error& e) {
        m_browser->SetException(this, e.what());
        return false;
    }
}
FB::variant FB::Npapi::NPJavascriptObject::NPO_addEventListener::exec( const std::vector<variant>& args )
{
    if (args.size() > 1 && args.size() < 4) {
        try {
            std::string evtName = "on" + args[0].convert_cast<std::string>();
            FB::JSObjectPtr method(args[1].convert_cast<FB::JSObjectPtr>());
            obj->getAPI()->registerEventMethod(evtName, method);
            return FB::variant();
        } catch (const std::bad_cast& e) {
            throw FB::invalid_arguments(e.what());
        }
    } else {
        throw FB::invalid_arguments();
    }
}

FB::variant FB::Npapi::NPJavascriptObject::NPO_removeEventListener::exec( const std::vector<variant>& args )
{
    if (args.size() > 1 && args.size() < 4) {
        try {
            std::string evtName = "on" + args[0].convert_cast<std::string>();
            FB::JSObjectPtr method(args[1].convert_cast<FB::JSObjectPtr>());
            obj->getAPI()->unregisterEventMethod(evtName, method);
            return FB::variant();
        } catch (const std::bad_cast& e) {
            throw FB::invalid_arguments(e.what());
        }
    } else {
        throw FB::invalid_arguments();
    }
}
bool NPJavascriptObject::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    VOID_TO_NPVARIANT(*result);
    try {
        std::string mName;
        if (name != NULL) {
            mName = m_browser->StringFromIdentifier(name);
        }
        std::vector<FB::variant> vArgs;
        for (unsigned int i = 0; i < argCount; i++) {
            vArgs.push_back(m_browser->getVariant(&args[i]));
        }

        // Default method call
        FB::variant ret = getAPI()->Invoke(mName, vArgs);
        m_browser->getNPVariant(result, ret);
        return true;
    } catch (const std::bad_cast&) {
        return false; // invalid object
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
    try {
        // Handle numeric identifiers
        if(!m_browser->IdentifierIsString(name)) {
            int32_t sIdx = m_browser->IntFromIdentifier(name);
            return getAPI()->HasProperty(sIdx);
        }

        std::string sName(m_browser->StringFromIdentifier(name));
        // We check for events of that name as well in order to allow setting of an event handler in the
        // old javascript style, i.e. plugin.onload = function() .....;

        if (sName == "addEventListener" || sName == "removeEventListener") {
            return true;
        } else if (sName != "toString" && getAPI()->HasMethodObject(sName))
            return true;
        else
            return !HasMethod(name) && (getAPI()->HasEvent(sName) || getAPI()->HasProperty(sName));
    } catch (const std::bad_cast&) {
        return false; // invalid object
    } catch (const script_error& e) {
        m_browser->SetException(this, e.what());
        return false;
    }
}

bool NPJavascriptObject::GetProperty(NPIdentifier name, NPVariant *result)
{
    try {
        FB::variant res;
        if (m_browser->IdentifierIsString(name)) {
            std::string sName(m_browser->StringFromIdentifier(name));
            if (sName == "addEventListener") {
                res = m_addEventFunc;
            } else if (sName == "removeEventListener") {
                res = m_removeEventFunc;
            } else if (getAPI()->HasEvent(sName)) {
                FB::JSObjectPtr tmp(getAPI()->getDefaultEventMethod(sName));
                if (tmp != NULL)
                    res = tmp;
            } else if (getAPI()->HasMethodObject(sName)) {
                res = getAPI()->GetMethodObject(sName);
            } else {
                res = getAPI()->GetProperty(sName);
            }
        } else {
            res = getAPI()->GetProperty(m_browser->IntFromIdentifier(name));
        }

        m_browser->getNPVariant(result, res);
        return true;
    } catch (const std::bad_cast&) {
        return false; // invalid object
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
            if (getAPI()->HasEvent(sName)) {
                if(value->type == NPVariantType_Null) {
                    FB::JSObjectPtr nullEvent;
                    getAPI()->setDefaultEventMethod(sName, nullEvent);
                } else if(value->type == NPVariantType_Object) {
                    FB::JSObjectPtr tmp(arg.cast<FB::JSObjectPtr>());
                    getAPI()->setDefaultEventMethod(sName, tmp);
                }
            } else if (getAPI()->HasMethodObject(sName)) {
                throw FB::script_error("This property cannot be changed");
            } else {
                getAPI()->SetProperty(sName, arg);
            }
        } else {
            getAPI()->SetProperty(m_browser->IntFromIdentifier(name), arg);
        }
        return true;
    } catch (const std::bad_cast&) {
        return false; // invalid object
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
            if (getAPI()->HasEvent(sName)) {
                FB::JSObjectPtr nullEvent;
                getAPI()->setDefaultEventMethod(sName, nullEvent);
            }
        }
        return false;
    } catch (const std::bad_cast&) {
        return false; // invalid object
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
        getAPI()->getMemberNames(memberList);
        *count = memberList.size() + 2;
        NPIdentifier *outList(NULL);
        outList = (NPIdentifier*)m_browser->MemAlloc((uint32_t)(sizeof(NPIdentifier) * *count));
        
        for (uint32_t i = 0; i < memberList.size(); i++) {
            outList[i] = m_browser->GetStringIdentifier(memberList.at(i).c_str());
        }
        outList[memberList.size()] = m_browser->GetStringIdentifier("addEventListener");
        outList[memberList.size() + 1] = m_browser->GetStringIdentifier("removeEventListener");
        *value = outList;
        return true;
    } catch (const std::bad_cast&) {
        *count = 0;
        return false; // invalid object
    } catch (const script_error& e) {
        *count = 0;
        m_browser->SetException(this, e.what());
        return false;
    }
}

bool NPJavascriptObject::Construct(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    try {
        // TODO: add support for constructing
        return false;
    } catch (const std::bad_cast&) {
        return false; // invalid object
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


