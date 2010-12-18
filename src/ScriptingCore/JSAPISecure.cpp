/**********************************************************\ 
Original Author: Richard Bateman

Created:    Oct 20, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#include "JSAPISecure.h"

FB::JSAPISecure::JSAPISecure(const FB::SecurityZone securityZone, const std::string& description) :
    FB::JSAPIAuto(description), m_zone(securityZone)
{
}

FB::JSAPISecure::~JSAPISecure()
{
}

void FB::JSAPISecure::registerMethod(const std::string& name, const FB::SecurityZone minZone, const FB::CallMethodFunctor& func)
{
    m_methodFunctorMap[name] = MethodFunctors(minZone, func);
}

void FB::JSAPISecure::registerProperty(const std::string& name, const FB::SecurityZone minZone, const FB::PropertyFunctors& propFuncs)
{
    m_propertyFunctorsMap[name] = PropertyFunctors(minZone, propFuncs.get, propFuncs.set);
}

void FB::JSAPISecure::getMemberNames(std::vector<std::string> &nameVector)
{
	getMemberNames(m_zone, nameVector);
}

void FB::JSAPISecure::getMemberNames( const SecurityZone securityLevel, std::vector<std::string> &nameVector )
{
    nameVector.clear();

    for (FB::MethodFunctorMap::iterator it = m_methodFunctorMap.begin();
        it != m_methodFunctorMap.end(); it++) {
		if (securityLevel >= it->second.zone)
			nameVector.push_back(it->first);
    }
    for (FB::PropertyFunctorsMap::iterator it = m_propertyFunctorsMap.begin();
        it != m_propertyFunctorsMap.end(); it++) {
		if (securityLevel >= it->second.zone)
	        nameVector.push_back(it->first);
    }
    for (FB::EventSingleMap::iterator it = m_defEventMap.begin();
        it != m_defEventMap.end(); it++) {
        nameVector.push_back(it->first);
    }
}

size_t FB::JSAPISecure::getMemberCount()
{
	return getMemberCount(m_zone);
}

size_t FB::JSAPISecure::getMemberCount( const SecurityZone securityLevel )
{
	size_t count = 0;
    for (FB::MethodFunctorMap::iterator it = m_methodFunctorMap.begin();
        it != m_methodFunctorMap.end(); it++) {
		if (securityLevel >= it->second.zone)
			count++;
    }
    for (FB::PropertyFunctorsMap::iterator it = m_propertyFunctorsMap.begin();
        it != m_propertyFunctorsMap.end(); it++) {
		if (securityLevel >= it->second.zone)
			count++;
    }
	count += m_eventMap.size();
	return count;
}

FB::variant FB::JSAPISecure::GetProperty( const std::string& propertyName )
{
	return GetProperty(m_zone, propertyName);
}

FB::variant FB::JSAPISecure::GetProperty( int idx )
{
    // We don't currently register members like this, so there is no built-in security for them.
    // Thus, to be secure, we simply disable them =]
    std::string id(boost::lexical_cast<std::string>(idx));
    AttributeMap::iterator fnd = m_attributes.find(id);
    if (fnd != m_attributes.end()) {
        return fnd->second.value;
    } else if (m_allowDynamicAttributes) {
        return FB::FBVoid();
    } else {
        throw invalid_member(boost::lexical_cast<std::string>(idx));
    }
}

FB::variant FB::JSAPISecure::GetProperty( const FB::SecurityZone &zone, const std::string& propertyName )
{
    if (propertyAccessible(zone, propertyName)) {
        return FB::JSAPIAuto::GetProperty(propertyName);
    } else if (m_allowDynamicAttributes) {
        return FB::FBVoid();
    } else {
        throw invalid_member(propertyName);
    }
}

FB::variant FB::JSAPISecure::GetProperty( const FB::SecurityZone &zone, int idx )
{
    // We don't currently register members like this, so there is no built-in security for them.
    // Thus, to be secure, we simply disable them =]
    throw invalid_member(boost::lexical_cast<std::string>(idx));
}

void FB::JSAPISecure::SetProperty( const std::string& propertyName, const variant& value )
{
	SetProperty(m_zone, propertyName, value);
}

void FB::JSAPISecure::SetProperty( int idx, const variant& value )
{
    // We don't currently register members like this, so there is no built-in security for them.
    // Thus, to be secure, we simply disable them unless dynamic attributes (which are not secured)
    // are enable
    if (m_allowDynamicAttributes)
        registerAttribute(boost::lexical_cast<std::string>(idx), value);
    else
        throw invalid_member(boost::lexical_cast<std::string>(idx));
}

void FB::JSAPISecure::SetProperty( const FB::SecurityZone &zone, const std::string& propertyName, const variant& value )
{
    if (propertyAccessible(zone, propertyName))
        FB::JSAPIAuto::SetProperty(propertyName, value);
    else if (m_allowDynamicAttributes && m_propertyFunctorsMap.find(propertyName) == m_propertyFunctorsMap.end())
        registerAttribute(propertyName, value);
    else
        throw invalid_member(propertyName);
}

void FB::JSAPISecure::SetProperty( const FB::SecurityZone &zone, int idx, const variant& value )
{
    // We don't currently register members like this, so there is no built-in security for them.
    // Thus, to be secure, we simply disable them unless dynamic attributes (which are not secured)
    // are enable
    if (m_allowDynamicAttributes)
        registerAttribute(boost::lexical_cast<std::string>(idx), value);
    else
        throw invalid_member(boost::lexical_cast<std::string>(idx));
}

FB::variant FB::JSAPISecure::Invoke( const std::string& methodName, const std::vector<variant>& args )
{
	return Invoke(m_zone, methodName, args);
}

FB::variant FB::JSAPISecure::Invoke( const FB::SecurityZone &zone, const std::string& methodName, const std::vector<variant>& args )
{
    if (methodAccessible(zone, methodName))
        return FB::JSAPIAuto::Invoke(methodName, args);
    else
        throw invalid_member(methodName);
}

bool FB::JSAPISecure::methodAccessible( SecurityZone m_zone, const std::string& methodName )
{
    MethodFunctorMap::iterator it = m_methodFunctorMap.find(methodName);
    if(it != m_methodFunctorMap.end())
        return (m_zone >= it->second.zone);
    else {
        return false;
    }
}

bool FB::JSAPISecure::propertyAccessible( SecurityZone m_zone, const std::string& propertyName )
{
    PropertyFunctorsMap::iterator it = m_propertyFunctorsMap.find(propertyName);
    if(it != m_propertyFunctorsMap.end()) {
        return (m_zone >= it->second.zone);
    } else {
        // generic attributes are accessible from any zone
        return m_attributes.find(propertyName) != m_attributes.end();
    }
}

bool FB::JSAPISecure::HasMethod( const std::string& methodName )
{
    return methodAccessible(m_zone, methodName);
}

bool FB::JSAPISecure::HasMethod( const FB::SecurityZone &zone, const std::string& methodName )
{
    return methodAccessible(zone, methodName);
}

bool FB::JSAPISecure::HasProperty( const std::string& propertyName )
{
    // To be able to set dynamic properties, we have to respond true always
    if (m_allowDynamicAttributes && !HasMethod(propertyName))
        return true;

    return propertyAccessible(m_zone, propertyName);
}

bool FB::JSAPISecure::HasProperty( const FB::SecurityZone &zone, const std::string& propertyName )
{
    // To be able to set dynamic properties, we have to respond true always
    if (m_allowDynamicAttributes && !HasMethod(propertyName))
        return true;

    return propertyAccessible(zone, propertyName);
}

bool FB::JSAPISecure::HasProperty( int idx )
{
    // To be able to set dynamic properties, we have to respond true always
    if (m_allowDynamicAttributes)
        return true;

    return m_attributes.find(boost::lexical_cast<std::string>(idx)) != m_attributes.end();
}

bool FB::JSAPISecure::HasProperty( const FB::SecurityZone &zone, int idx )
{
    // To be able to set dynamic properties, we have to respond true always
    if (m_allowDynamicAttributes)
        return true;

    return m_attributes.find(boost::lexical_cast<std::string>(idx)) != m_attributes.end();
}
