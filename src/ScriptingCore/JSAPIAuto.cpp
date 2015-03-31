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
#include "JSFunction.h"
#include "JSEvent.h"
#include <cassert>
#include <mutex>
#include <memory>

#include "JSAPIAuto.h"

bool FB::JSAPIAuto::s_allowRemoveProperties = false;

FB::JSAPIAuto::JSAPIAuto(std::string description)
  : FB::JSAPIImpl(SecurityScope_Public),
    m_description(description),
    m_allowRemoveProperties(FB::JSAPIAuto::s_allowRemoveProperties)
{
    init();
}

FB::JSAPIAuto::JSAPIAuto( const SecurityZone& securityLevel, std::string description /*= "<JSAPI-Auto Secure Javascript Object>"*/ )
  : FB::JSAPIImpl(securityLevel),
    m_description(description),
    m_allowRemoveProperties(FB::JSAPIAuto::s_allowRemoveProperties)
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
}

FB::JSAPIAuto::~JSAPIAuto()
{

}

void FB::JSAPIAuto::registerMethod(std::string name, const CallMethodFunctor& func)
{
    std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);
    m_methodFunctorMap[name] = func;
    m_zoneMap[name] = getZone();
}

void FB::JSAPIAuto::unregisterMethod( std::string name )
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

void FB::JSAPIAuto::registerProperty(std::string name, const PropertyFunctors& propFuncs)
{
    std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);
    m_propertyFunctorsMap[name] = propFuncs;
    m_zoneMap[name] = getZone();
}

void FB::JSAPIAuto::unregisterProperty( const std::wstring& name )
{
    unregisterProperty(FB::wstring_to_utf8(name));
}

void FB::JSAPIAuto::unregisterProperty( std::string name )
{
    FB::PropertyFunctorsMap::iterator fnd = m_propertyFunctorsMap.find(name);
    if (fnd != m_propertyFunctorsMap.end()) {
        m_propertyFunctorsMap.erase(name);
        m_zoneMap.erase(name);
    }
}

void FB::JSAPIAuto::getMemberNames(std::vector<std::string> &nameVector) const
{
    std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);
    nameVector.clear();
    for (ZoneMap::const_iterator it = m_zoneMap.begin(); it != m_zoneMap.end(); ++it) {
        if (getZone() >= it->second)
            nameVector.emplace_back(it->first);
    }
}

size_t FB::JSAPIAuto::getMemberCount() const
{
    std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);
    size_t count = 0;
    for (ZoneMap::const_iterator it = m_zoneMap.begin(); it != m_zoneMap.end(); ++it) {
        if (getZone() >= it->second)
            ++count;
    }
    return count;
}

bool FB::JSAPIAuto::HasMethod(std::string methodName) const
{
    std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);
    if(!m_valid)
        return false;

    return (m_methodFunctorMap.find(methodName) != m_methodFunctorMap.end()) && memberAccessible(m_zoneMap.find(methodName));
}

bool FB::JSAPIAuto::HasProperty(std::string propertyName) const
{
    std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);
    if(!m_valid)
        return false;

    return m_propertyFunctorsMap.find(propertyName) != m_propertyFunctorsMap.end()
        || m_attributes.find(propertyName) != m_attributes.end();
}

bool FB::JSAPIAuto::HasProperty(int idx) const
{
    std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);
    if(!m_valid)
        return false;

    return m_attributes.find(std::to_string(idx)) != m_attributes.end();
}

FB::variantPromise FB::JSAPIAuto::GetProperty(std::string propertyName)
{
    std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);
    if(!m_valid)
        throw object_invalidated();

    ZoneMap::const_iterator zoneName = m_zoneMap.find(propertyName);
    PropertyFunctorsMap::const_iterator it = m_propertyFunctorsMap.find(propertyName);
    if(it != m_propertyFunctorsMap.end() && memberAccessible(zoneName)) {
        return it->second.get();
    } else if (memberAccessible(zoneName)) {
        AttributeMap::iterator fnd = m_attributes.find(propertyName);
        if (fnd != m_attributes.end()) {
            return fnd->second.value;
        } else {
            throw invalid_member(propertyName);
        }
    } else {
        throw invalid_member(propertyName);
    }
}

void FB::JSAPIAuto::SetProperty(std::string propertyName, const variant& value)
{
    std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);
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
    } else if (m_attributes.find(propertyName) != m_attributes.end() && !m_attributes[propertyName].readonly) {
        registerAttribute(propertyName, value);
    } else {
        throw invalid_member(propertyName);
    }
}

void FB::JSAPIAuto::RemoveProperty(std::string propertyName)
{
    std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);
    if(!m_valid)
        throw object_invalidated();

    // If there is nothing with this name available in the current security context,
    // we throw an exception -- whether or not a real property exists
    if (!memberAccessible(m_zoneMap.find(propertyName)))
        throw invalid_member(propertyName);

    if(m_allowRemoveProperties && m_propertyFunctorsMap.find(propertyName) != m_propertyFunctorsMap.end()) {
        unregisterProperty(propertyName);
    }

    // If nothing is found matching, we'll just let it slide -- no sense causing exceptions
    // when the end goal is reached already.
}

FB::variantPromise FB::JSAPIAuto::GetProperty(int idx)
{
    std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);
    if(!m_valid)
        throw object_invalidated();

    std::string id = std::to_string(idx);
    AttributeMap::iterator fnd = m_attributes.find(id);
    if (fnd != m_attributes.end() && memberAccessible(m_zoneMap.find(id))) {
        return fnd->second.value;
    } else {
        throw invalid_member(std::to_string(idx));
    }

    // This method should be overridden to access properties in an array style from javascript,
    // i.e. var value = pluginObj[45]; would call GetProperty(45)
    // Default is to throw "invalid member" unless m_attributes has something matching
}

void FB::JSAPIAuto::SetProperty(int idx, const variant& value)
{
    if (!m_valid)
        throw object_invalidated();

    std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);

    std::string id(std::to_string(idx));
    throw invalid_member(FB::variant(idx).convert_cast<std::string>());
}

void FB::JSAPIAuto::RemoveProperty(int idx)
{
    if (!m_valid)
        throw object_invalidated();

    std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);

    std::string id(std::to_string(idx));
    throw invalid_member(FB::variant(idx).convert_cast<std::string>());
}

FB::variantPromise FB::JSAPIAuto::Invoke(std::string methodName, const std::vector<variant> &args)
{
    std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);
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


void FB::JSAPIAuto::registerAttribute( const std::string &name, const FB::variant& value, bool readonly /*= false*/ )
{
    std::unique_lock<std::recursive_mutex> lock(m_zoneMutex);
    Attribute attr = {value, readonly};
    m_attributes[name] = attr;
    m_zoneMap[name] = getZone();
}

void FB::JSAPIAuto::unregisterAttribute( std::string name )
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

FB::variantPromise FB::JSAPIAuto::getAttribute( std::string name )
{
    if (m_attributes.find(name) != m_attributes.end()) {
        return m_attributes[name].value;
    }
    return FB::variant();
}

void FB::JSAPIAuto::setAttribute( std::string name, const FB::variant& value )
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

void FB::JSAPIAuto::FireJSEvent( std::string eventName, const FB::VariantMap &members, const FB::VariantList &arguments )
{
    JSAPIImpl::FireJSEvent(eventName, members, arguments);
    FB::variant evt;
    try {
        evt = m_attributes.at(eventName).value;
    } catch (...) {};
    if (evt.is_of_type<FB::JSObjectPtr>()) {
        VariantList args;
        args.emplace_back(FB::CreateEvent(shared_from_this(), eventName, members, arguments));
        try {
            evt.cast<JSObjectPtr>()->Invoke("", args);
        } catch (...) {
            // Apparently either this isn't really an event handler or something failed.
        }
    }
}

void FB::JSAPIAuto::fireAsyncEvent( std::string eventName, const std::vector<variant>& args )
{
    JSAPIImpl::fireAsyncEvent(eventName, args);
    FB::variant evt;
    try {
        evt = m_attributes.at(eventName).value;
    } catch (...) {};
    if (evt.is_of_type<FB::JSObjectPtr>()) {
        try {
            FB::JSObjectPtr handler(evt.cast<JSObjectPtr>());
            if (handler) {
                handler->Invoke("", args);
            }
        } catch (...) {
            // Apparently either this isn't really an event handler or something failed.
        }
    }
}
