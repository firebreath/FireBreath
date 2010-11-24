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

FB::variant FB::JSAPISecure::GetProperty( const std::string& propertyName )
{
    if (propertyAccessible(m_zone, propertyName))
        return FB::JSAPIAuto::GetProperty(propertyName);
    else
        throw invalid_member(propertyName);
}

FB::variant FB::JSAPISecure::GetProperty( int idx )
{
    // We don't currently register members like this, so there is no built-in security for them.
    // Thus, to be secure, we simply disable them =]
    throw invalid_member(boost::lexical_cast<std::string>(idx));
}

FB::variant FB::JSAPISecure::GetProperty( const FB::SecurityZone &zone, const std::string& propertyName )
{
    if (propertyAccessible(zone, propertyName))
        return FB::JSAPIAuto::GetProperty(propertyName);
    else
        throw invalid_member(propertyName);
}

FB::variant FB::JSAPISecure::GetProperty( const FB::SecurityZone &zone, int idx )
{
    // We don't currently register members like this, so there is no built-in security for them.
    // Thus, to be secure, we simply disable them =]
    throw invalid_member(boost::lexical_cast<std::string>(idx));
}

void FB::JSAPISecure::SetProperty( const std::string& propertyName, const variant& value )
{
    if (propertyAccessible(m_zone, propertyName))
        FB::JSAPIAuto::SetProperty(propertyName, value);
    else
        throw invalid_member(propertyName);
}

void FB::JSAPISecure::SetProperty( int idx, const variant& value )
{
    // We don't currently register members like this, so there is no built-in security for them.
    // Thus, to be secure, we simply disable them =]
    throw invalid_member(boost::lexical_cast<std::string>(idx));
}

void FB::JSAPISecure::SetProperty( const FB::SecurityZone &zone, const std::string& propertyName, const variant& value )
{
    if (propertyAccessible(zone, propertyName))
        FB::JSAPIAuto::SetProperty(propertyName, value);
    else
        throw invalid_member(propertyName);
}

void FB::JSAPISecure::SetProperty( const FB::SecurityZone &zone, int idx, const variant& value )
{
    // We don't currently register members like this, so there is no built-in security for them.
    // Thus, to be secure, we simply disable them =]
    throw invalid_member(boost::lexical_cast<std::string>(idx));
}

FB::variant FB::JSAPISecure::Invoke( const std::string& methodName, const std::vector<variant>& args )
{
    if (methodAccessible(m_zone, methodName))
        return FB::JSAPIAuto::Invoke(methodName, args);
    else
        throw invalid_member(methodName);
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
    if(it == m_methodFunctorMap.end())
        return false;
    else {
        return (m_zone >= it->second.zone);
    }
}

bool FB::JSAPISecure::propertyAccessible( SecurityZone m_zone, const std::string& propertyName )
{
    PropertyFunctorsMap::iterator it = m_propertyFunctorsMap.find(propertyName);
    if(it == m_propertyFunctorsMap.end())
        return false;
    else {
        return (m_zone >= it->second.zone);
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
    return propertyAccessible(m_zone, propertyName);
}

bool FB::JSAPISecure::HasProperty( const FB::SecurityZone &zone, const std::string& propertyName )
{
    return propertyAccessible(zone, propertyName);
}

bool FB::JSAPISecure::HasProperty( int idx )
{
    return false;
}

bool FB::JSAPISecure::HasProperty( const FB::SecurityZone &zone, int idx )
{
    return false;
}
