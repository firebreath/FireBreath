/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Nov 23, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
            
Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include <boost/make_shared.hpp>

#include "JSAPIProxy.h"

FB::JSAPIProxyPtr FB::JSAPIProxy::create( const FB::JSAPIPtr &inner )
{
    // This is necessary because you can't use shared_from_this in the constructor
    FB::JSAPIProxyPtr ptr(new FB::JSAPIProxy(inner));
    inner->registerProxy(ptr);

    return ptr;
}

FB::JSAPIProxyPtr FB::JSAPIProxy::create( const FB::JSAPIWeakPtr &inner )
{
    // This is necessary because you can't use shared_from_this in the constructor
    FB::JSAPIProxyPtr ptr(new FB::JSAPIProxy(inner));
    FB::JSAPIPtr tmp = inner.lock();
    if (tmp)
        tmp->registerProxy(ptr);

    return ptr;
}

FB::JSAPIProxyPtr FB::JSAPIProxy::create( const SecurityZone& securityLevel, const FB::JSAPIPtr &inner )
{
    // This is necessary because you can't use shared_from_this in the constructor
    FB::JSAPIProxyPtr ptr(new FB::JSAPIProxy(securityLevel, inner));
    inner->registerProxy(ptr);

    return ptr;
}

FB::JSAPIProxyPtr FB::JSAPIProxy::create( const SecurityZone& securityLevel, const FB::JSAPIWeakPtr &inner )
{
    // This is necessary because you can't use shared_from_this in the constructor
    FB::JSAPIProxyPtr ptr(new FB::JSAPIProxy(securityLevel, inner));
    FB::JSAPIPtr tmp = inner.lock();
    if (tmp)
        tmp->registerProxy(ptr);

    return ptr;
}

FB::JSAPIProxy::JSAPIProxy( const FB::JSAPIPtr &inner )
    : m_api(inner), m_apiWeak(inner)
{
    if (!inner) {
        throw FB::script_error("Invalid inner JSAPI object passed to proxy");
    }
}

FB::JSAPIProxy::JSAPIProxy( const FB::JSAPIWeakPtr &inner )
    : m_apiWeak(inner)
{

}

FB::JSAPIProxy::JSAPIProxy( const SecurityZone& securityLevel, const FB::JSAPIPtr &inner )
    : FB::JSAPI(securityLevel), m_api(inner), m_apiWeak(inner)
{
    if (!inner) {
        throw FB::script_error("Invalid inner JSAPI object passed to proxy");
    }
}

FB::JSAPIProxy::JSAPIProxy( const SecurityZone& securityLevel, const FB::JSAPIWeakPtr &inner )
    : FB::JSAPI(securityLevel), m_apiWeak(inner)
{

}

FB::JSAPIProxy::~JSAPIProxy( void )
{
}

void FB::JSAPIProxy::changeObject( const FB::JSAPIWeakPtr &inner )
{
    if (FB::JSAPIPtr ptr = m_apiWeak.lock()) {
        ptr->unregisterProxy(shared_ptr());
    }
    this->m_api.reset();
    this->m_apiWeak = inner;
    FB::JSAPIPtr ptr = inner.lock();
    if (ptr)
        ptr->registerProxy(shared_ptr());
}

void FB::JSAPIProxy::changeObject( const FB::JSAPIPtr &inner )
{
    if (FB::JSAPIPtr ptr = m_apiWeak.lock()) {
        ptr->unregisterProxy(shared_ptr());
    }
    this->m_api = inner;
    this->m_apiWeak = inner;
    inner->registerProxy(shared_ptr());
}

void FB::JSAPIProxy::reset()
{
    if (FB::JSAPIPtr ptr = m_apiWeak.lock()) {
        ptr->unregisterProxy(shared_ptr());
    }
    
}

void FB::JSAPIProxy::invalidate()
{
    getAPI()->invalidate();
}

void FB::JSAPIProxy::getMemberNames( std::vector<std::string> &nameVector ) const
{
    FB::scoped_zonelock _l(getAPI(), getZone());
    getAPI()->getMemberNames(nameVector);
}

size_t FB::JSAPIProxy::getMemberCount() const
{
    FB::scoped_zonelock _l(getAPI(), getZone());
    return getAPI()->getMemberCount();
}

bool FB::JSAPIProxy::HasMethod( const std::wstring& methodName ) const
{
    FB::scoped_zonelock _l(getAPI(), getZone());
    return getAPI()->HasMethod(methodName);
}

bool FB::JSAPIProxy::HasMethod( const std::string& methodName ) const
{
    FB::scoped_zonelock _l(getAPI(), getZone());
    return getAPI()->HasMethod(methodName);
}

bool FB::JSAPIProxy::HasProperty( const std::wstring& propertyName ) const
{
    if (propertyName == L"expired")
        return true;
    FB::scoped_zonelock _l(getAPI(), getZone());
    return getAPI()->HasProperty(propertyName);
}

bool FB::JSAPIProxy::HasProperty( const std::string& propertyName ) const
{
    if (propertyName == "expired")
        return true;
    FB::scoped_zonelock _l(getAPI(), getZone());
    return getAPI()->HasProperty(propertyName);
}

bool FB::JSAPIProxy::HasProperty( int idx ) const
{
    FB::scoped_zonelock _l(getAPI(), getZone());
    return getAPI()->HasProperty(idx);
}

bool FB::JSAPIProxy::HasEvent( const std::string& eventName ) const
{
    FB::scoped_zonelock _l(getAPI(), getZone());
    return getAPI()->HasEvent(eventName);
}

bool FB::JSAPIProxy::HasEvent( const std::wstring& eventName ) const
{
    FB::scoped_zonelock _l(getAPI(), getZone());
    return getAPI()->HasEvent(eventName);
}

FB::variant FB::JSAPIProxy::GetProperty( const std::wstring& propertyName )
{
    if (propertyName == L"expired")
        return isExpired();
    FB::scoped_zonelock _l(getAPI(), getZone());
    return getAPI()->GetProperty(propertyName);
}

FB::variant FB::JSAPIProxy::GetProperty( const std::string& propertyName )
{
    if (propertyName == "expired")
        return isExpired();
    FB::scoped_zonelock _l(getAPI(), getZone());
    return getAPI()->GetProperty(propertyName);
}

FB::variant FB::JSAPIProxy::GetProperty( int idx )
{
    FB::scoped_zonelock _l(getAPI(), getZone());
    return getAPI()->GetProperty(idx);
}

void FB::JSAPIProxy::SetProperty( const std::wstring& propertyName, const variant& value )
{
    FB::scoped_zonelock _l(getAPI(), getZone());
    getAPI()->SetProperty(propertyName, value);
}

void FB::JSAPIProxy::SetProperty( const std::string& propertyName, const variant& value )
{
    FB::scoped_zonelock _l(getAPI(), getZone());
    getAPI()->SetProperty(propertyName, value);
}

void FB::JSAPIProxy::SetProperty( int idx, const variant& value )
{
    FB::scoped_zonelock _l(getAPI(), getZone());
    getAPI()->SetProperty(idx, value);
}

FB::variant FB::JSAPIProxy::Invoke( const std::wstring& methodName, const std::vector<variant>& args )
{
    FB::scoped_zonelock _l(getAPI(), getZone());
    return getAPI()->Invoke(methodName, args);
}

FB::variant FB::JSAPIProxy::Invoke( const std::string& methodName, const std::vector<variant>& args )
{
    FB::scoped_zonelock _l(getAPI(), getZone());
    return getAPI()->Invoke(methodName, args);
}

