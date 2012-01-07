/**********************************************************\
Original Author: Georg Fritzsche

Created:    November 7, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#include "utf8_tools.h"
#include "boost/thread/mutex.hpp"
#include "boost/make_shared.hpp"
#include "JSFunction.h"
#include "JSEvent.h"
#include <cassert>
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include "JSAPIAuto.h"

bool FB::JSAPIAuto::s_allowDynamicAttributes = true;
bool FB::JSAPIAuto::s_allowRemoveProperties = false;
bool FB::JSAPIAuto::s_allowMethodObjects = true;

FB::JSAPIAuto::JSAPIAuto(const std::string& description)
  : FB::JSAPIImpl(SecurityScope_Public),
    m_description(description),
    m_allowDynamicAttributes(FB::JSAPIAuto::s_allowDynamicAttributes),
    m_allowRemoveProperties(FB::JSAPIAuto::s_allowRemoveProperties),
    m_allowMethodObjects(FB::JSAPIAuto::s_allowMethodObjects)
{
    init();
}

FB::JSAPIAuto::JSAPIAuto( const SecurityZone& securityLevel, const std::string& description /*= "<JSAPI-Auto Secure Javascript Object>"*/ )
  : FB::JSAPIImpl(securityLevel),
    m_description(description),
    m_allowDynamicAttributes(FB::JSAPIAuto::s_allowDynamicAttributes),
    m_allowRemoveProperties(FB::JSAPIAuto::s_allowRemoveProperties),
    m_allowMethodObjects(FB::JSAPIAuto::s_allowMethodObjects)
{
    init();
}

void FB::JSAPIAuto::init( )
{
    {
        scoped_zonelock _l(this, SecurityScope_Public);
        registerMethod("toString",  make_method(this, &JSAPIAuto::ToString));
        registerMethod("getAttribute",  make_method(this, &JSAPIAuto::getAttribute));
        registerMethod("setAttribute",  make_method(this, &JSAPIAuto::setAttribute));

        registerProperty("value", make_property(this, &JSAPIAuto::ToString));
        registerProperty("valid", make_property(this, &JSAPIAuto::get_valid));
    }

    setReserved("offsetWidth");
    setReserved("offsetHeight");
    setReserved("width");
    setReserved("height");
    setReserved("attributes");
    setReserved("nodeType");
    setReserved("namespaceURI");
    setReserved("localName");
    setReserved("wrappedJSObject");
    setReserved("prototype");
    setReserved("style");
    setReserved("id");
    setReserved("constructor");
}

FB::JSAPIAuto::~JSAPIAuto()
{

}

void FB::JSAPIAuto::registerMethod(const std::string& name, const CallMethodFunctor& func)
{
    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);
    m_methodFunctorMap[name] = func;
    m_zoneMap[name] = getZone();
}

void FB::JSAPIAuto::unregisterMethod( const std::string& name )
{
    FB::MethodFunctorMap::iterator fnd = m_methodFunctorMap.find(name);
    if (fnd != m_methodFunctorMap.end()) {
        m_methodFunctorMap.erase(name);
        m_zoneMap.erase(name);
    }
}

void FB::JSAPIAuto::registerProperty(const std::wstring& name, const PropertyFunctors& func)
{
    registerProperty(FB::wstring_to_utf8(name), func);
}

void FB::JSAPIAuto::registerProperty(const std::string& name, const PropertyFunctors& propFuncs)
{
    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);
    m_propertyFunctorsMap[name] = propFuncs;
    m_zoneMap[name] = getZone();
}

void FB::JSAPIAuto::unregisterProperty( const std::wstring& name )
{
    unregisterProperty(FB::wstring_to_utf8(name));
}

void FB::JSAPIAuto::unregisterProperty( const std::string& name )
{
    FB::PropertyFunctorsMap::iterator fnd = m_propertyFunctorsMap.find(name);
    if (fnd != m_propertyFunctorsMap.end()) {
        m_propertyFunctorsMap.erase(name);
        m_zoneMap.erase(name);
    }
}

void FB::JSAPIAuto::getMemberNames(std::vector<std::string> &nameVector) const
{
    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);
    nameVector.clear();
    for (ZoneMap::const_iterator it = m_zoneMap.begin(); it != m_zoneMap.end(); ++it) {
        if (getZone() >= it->second)
            nameVector.push_back(it->first);
    }
}

size_t FB::JSAPIAuto::getMemberCount() const
{
    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);
    size_t count = 0;
    for (ZoneMap::const_iterator it = m_zoneMap.begin(); it != m_zoneMap.end(); ++it) {
        if (getZone() >= it->second)
            ++count;
    }
    return count;
}

bool FB::JSAPIAuto::HasMethod(const std::string& methodName) const
{
    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);
    if(!m_valid)
        return false;

    return (m_methodFunctorMap.find(methodName) != m_methodFunctorMap.end()) && memberAccessible(m_zoneMap.find(methodName));
}

bool FB::JSAPIAuto::HasMethodObject( const std::string& methodObjName ) const
{
    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);

    return m_allowMethodObjects && HasMethod(methodObjName);
}

bool FB::JSAPIAuto::HasProperty(const std::string& propertyName) const
{
    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);
    if(!m_valid)
        return false;

    // To be able to set dynamic properties, we have to respond true always
    if (m_allowDynamicAttributes && !HasMethod(propertyName) && !isReserved(propertyName))
        return true;
    else if (m_allowMethodObjects && HasMethod(propertyName) && memberAccessible(m_zoneMap.find(propertyName)))
        return true;

    return m_propertyFunctorsMap.find(propertyName) != m_propertyFunctorsMap.end()
        || m_attributes.find(propertyName) != m_attributes.end();
}

bool FB::JSAPIAuto::HasProperty(int idx) const
{
    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);
    if(!m_valid)
        return false;

    // To be able to set dynamic properties, we have to respond true always
    if (m_allowDynamicAttributes)
        return true;

    return m_attributes.find(boost::lexical_cast<std::string>(idx)) != m_attributes.end();
}

FB::variant FB::JSAPIAuto::GetProperty(const std::string& propertyName)
{
    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);
    if(!m_valid)
        throw object_invalidated();

    ZoneMap::const_iterator zoneName = m_zoneMap.find(propertyName);
    PropertyFunctorsMap::const_iterator it = m_propertyFunctorsMap.find(propertyName);
    if(it != m_propertyFunctorsMap.end() && memberAccessible(zoneName)) {
        return it->second.get();
    } else if (memberAccessible(zoneName)) {
        if (HasMethodObject(propertyName))
            return GetMethodObject(propertyName);

        AttributeMap::iterator fnd = m_attributes.find(propertyName);
        if (fnd != m_attributes.end())
            return fnd->second.value;
        else if (m_allowDynamicAttributes) {
            return FB::FBVoid(); // If we allow dynamic attributes then we need to
                                 // return void if the property doesn't exist;
                                 // otherwise checking a property will throw an exception
        } else {
            throw invalid_member(propertyName);
        }
    } else {
        if (m_allowDynamicAttributes) {
            return FB::FBVoid();
        } else {
            throw invalid_member(propertyName);
        }
    }
}

void FB::JSAPIAuto::SetProperty(const std::string& propertyName, const variant& value)
{
    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);
    if(!m_valid)
        throw object_invalidated();

    PropertyFunctorsMap::iterator it = m_propertyFunctorsMap.find(propertyName);
    // Note that if an explicit property exists but is not accessible in the current security context,
    // we throw an exception.
    if(it != m_propertyFunctorsMap.end()) {
        if (memberAccessible(m_zoneMap.find(propertyName))) {
            try {
                it->second.set(value);
            } catch (const FB::bad_variant_cast& ex) {
                std::string errorMsg("Could not convert from ");
                errorMsg += ex.from;
                errorMsg += " to ";
                errorMsg += ex.to;
                throw FB::invalid_arguments(errorMsg);
            }
        } else {
            throw invalid_member(propertyName);
        }
    } else if (m_allowDynamicAttributes || (m_attributes.find(propertyName) != m_attributes.end() && !m_attributes[propertyName].readonly)) {
        registerAttribute(propertyName, value);
    } else {
        throw invalid_member(propertyName);
    }
}

void FB::JSAPIAuto::RemoveProperty(const std::string& propertyName)
{
    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);
    if(!m_valid)
        throw object_invalidated();

    // If there is nothing with this name available in the current security context,
    // we throw an exception -- whether or not a real property exists
    if (!memberAccessible(m_zoneMap.find(propertyName)))
        throw invalid_member(propertyName);

    if(m_allowRemoveProperties && m_propertyFunctorsMap.find(propertyName) != m_propertyFunctorsMap.end()) {
        unregisterProperty(propertyName);
    } else if (m_allowDynamicAttributes && m_attributes.find(propertyName) != m_attributes.end()
               && !m_attributes[propertyName].readonly) {
        unregisterAttribute(propertyName);
    }

    // If nothing is found matching, we'll just let it slide -- no sense causing exceptions
    // when the end goal is reached already.
}

FB::variant FB::JSAPIAuto::GetProperty(int idx)
{
    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);
    if(!m_valid)
        throw object_invalidated();

    std::string id = boost::lexical_cast<std::string>(idx);
    AttributeMap::iterator fnd = m_attributes.find(id);
    if (fnd != m_attributes.end() && memberAccessible(m_zoneMap.find(id)))
        return fnd->second.value;
    else if (m_allowDynamicAttributes) {
        return FB::FBVoid(); // If we allow dynamic attributes then we need to
                             // return void if the property doesn't exist;
                             // otherwise checking a property will throw an exception
    } else {
        throw invalid_member(boost::lexical_cast<std::string>(idx));
    }

    // This method should be overridden to access properties in an array style from javascript,
    // i.e. var value = pluginObj[45]; would call GetProperty(45)
    // Default is to throw "invalid member" unless m_attributes has something matching
}

void FB::JSAPIAuto::SetProperty(int idx, const variant& value)
{
    if (!m_valid)
        throw object_invalidated();

    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);

    std::string id(boost::lexical_cast<std::string>(idx));
    if (m_allowDynamicAttributes || (m_attributes.find(id) != m_attributes.end() && !m_attributes[id].readonly)) {
        registerAttribute(id, value);
    } else {
        throw invalid_member(FB::variant(idx).convert_cast<std::string>());
    }
}

void FB::JSAPIAuto::RemoveProperty(int idx)
{
    if (!m_valid)
        throw object_invalidated();

    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);

    std::string id(boost::lexical_cast<std::string>(idx));
    if (m_allowDynamicAttributes && m_attributes.find(id) != m_attributes.end() && !m_attributes[id].readonly) {
        unregisterAttribute(id);
    } else {
        throw invalid_member(FB::variant(idx).convert_cast<std::string>());
    }
}

FB::variant FB::JSAPIAuto::Invoke(const std::string& methodName, const std::vector<variant> &args)
{
    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);
    if(!m_valid)
        throw object_invalidated();

    if (memberAccessible(m_zoneMap.find(methodName))) {
        try {
            MethodFunctorMap::iterator it = m_methodFunctorMap.find(methodName);
            if(it == m_methodFunctorMap.end())
                throw invalid_member(methodName);

            return it->second.call(args);
        } catch (const FB::bad_variant_cast& ex) {
            std::string errorMsg("Could not convert from ");
            errorMsg += ex.from;
            errorMsg += " to ";
            errorMsg += ex.to;
            throw FB::invalid_arguments(errorMsg);
        }
    } else {
        throw invalid_member(methodName);
    }
}

FB::variant FB::JSAPIAuto::Construct(const std::vector<variant> &args)
{
    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);
    if(!m_valid)
        throw object_invalidated();

    throw invalid_member("constructor");
}

FB::JSAPIPtr FB::JSAPIAuto::GetMethodObject( const std::string& methodObjName )
{
    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);
    if(!m_valid)
        throw object_invalidated();

    if (memberAccessible(m_zoneMap.find(methodObjName)) && HasMethod(methodObjName)) {
        MethodObjectMap::const_iterator fnd = m_methodObjectMap.find(boost::make_tuple(methodObjName, getZone()));
        if (fnd != m_methodObjectMap.end()) {
            return fnd->second;
        } else {
            FB::JSFunctionPtr ptr(boost::make_shared<FB::JSFunction>(shared_from_this(), methodObjName, getZone()));
            m_methodObjectMap[boost::make_tuple(methodObjName, getZone())] = ptr;
            return ptr;
        }
    } else {
        throw invalid_member(methodObjName);
    }
}

void FB::JSAPIAuto::registerAttribute( const std::string &name, const FB::variant& value, bool readonly /*= false*/ )
{
    boost::recursive_mutex::scoped_lock lock(m_zoneMutex);
    Attribute attr = {value, readonly};
    m_attributes[name] = attr;
    m_zoneMap[name] = getZone();
}

void FB::JSAPIAuto::unregisterAttribute( const std::string& name )
{
    AttributeMap::iterator fnd = m_attributes.find(name);
    if ( fnd != m_attributes.end() ) {
        if (fnd->second.readonly ) {
            throw FB::script_error("Cannot remove read-only property " + name);
        } else {
            m_attributes.erase(fnd);
            m_zoneMap.erase(name);
        }
    } else {
        return; // No attribute of that name? success!
    }
}

FB::variant FB::JSAPIAuto::getAttribute( const std::string& name )
{
    if (m_attributes.find(name) != m_attributes.end()) {
        return m_attributes[name].value;
    }
    return FB::FBVoid();
}

void FB::JSAPIAuto::setAttribute( const std::string& name, const FB::variant& value )
{
    AttributeMap::iterator fnd = m_attributes.find(name);
    if (fnd == m_attributes.end() || !fnd->second.readonly) {
        Attribute attr = {value, false};
        m_attributes[name] = attr;
        m_zoneMap[name] = getZone();
    } else {
        throw FB::script_error("Cannot set read-only property " + name);
    }
}

void FB::JSAPIAuto::FireJSEvent( const std::string& eventName, const FB::VariantMap &members, const FB::VariantList &arguments )
{
    JSAPIImpl::FireJSEvent(eventName, members, arguments);
    FB::variant evt(getAttribute(eventName));
    if (evt.is_of_type<FB::JSObjectPtr>()) {
        VariantList args;
        args.push_back(FB::CreateEvent(shared_from_this(), eventName, members, arguments));
        try {
            evt.cast<JSObjectPtr>()->InvokeAsync("", args);
        } catch (...) {
            // Apparently either this isn't really an event handler or something failed.
        }
    }
}

void FB::JSAPIAuto::fireAsyncEvent( const std::string& eventName, const std::vector<variant>& args )
{
    JSAPIImpl::fireAsyncEvent(eventName, args);
    FB::variant evt(getAttribute(eventName));
    if (evt.is_of_type<FB::JSObjectPtr>()) {
        try {
            FB::JSObjectPtr handler(evt.cast<JSObjectPtr>());
            if (handler) {
                handler->InvokeAsync("", args);
            }
        } catch (...) {
            // Apparently either this isn't really an event handler or something failed.
        }
    }
}
