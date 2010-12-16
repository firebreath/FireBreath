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
#include "APITypes.h"

#include "JSAPISecureProxy.h"

boost::shared_ptr<FB::JSAPISecureProxy> FB::JSAPISecureProxy::create( const FB::SecurityZone zone, const FB::JSAPIPtr &inner )
{
    boost::shared_ptr<FB::JSAPISecureProxy> ptr(new FB::JSAPISecureProxy(zone, inner));
    inner->registerProxy(ptr);
    // This is necessary because you can't use shared_from_this in the constructor
    return ptr;
}

boost::shared_ptr<FB::JSAPISecureProxy> FB::JSAPISecureProxy::create( const FB::SecurityZone zone, const FB::JSAPIWeakPtr &inner )
{
    boost::shared_ptr<FB::JSAPISecureProxy> ptr(new FB::JSAPISecureProxy(zone, inner));
    inner.lock()->registerProxy(ptr);
    // This is necessary because you can't use shared_from_this in the constructor
    return ptr;
}

FB::JSAPISecureProxy::JSAPISecureProxy( const FB::SecurityZone zone, const FB::JSAPIPtr &inner )
    : m_zone(zone), m_api(FB::ptr_cast<FB::JSAPISecure>(inner))
{
    m_apiWeak = m_api;
    if (!m_api) {
        throw FB::script_error("Invalid inner JSAPISecure object passed to proxy");
    }
}

FB::JSAPISecureProxy::JSAPISecureProxy( const FB::SecurityZone zone, const FB::JSAPIWeakPtr &inner )
    : m_zone(zone)
{
    FB::JSAPIPtr tmp = inner.lock();
    if (!tmp) {
        throw FB::script_error("Invalid inner JSAPISecure object passed to proxy");
    }
    m_apiWeak = FB::ptr_cast<FB::JSAPISecure>(tmp);
}

FB::JSAPISecureProxy::~JSAPISecureProxy( void )
{
}

void FB::JSAPISecureProxy::invalidate()
{
    getInnerAPI()->invalidate();
}

void FB::JSAPISecureProxy::getMemberNames( std::vector<std::string> &nameVector )
{
    getInnerAPI()->getMemberNames(m_zone, nameVector);
}

size_t FB::JSAPISecureProxy::getMemberCount()
{
    return getInnerAPI()->getMemberCount(m_zone);
}

bool FB::JSAPISecureProxy::HasMethod( const std::string& methodName )
{
    return getInnerAPI()->HasMethod(m_zone, methodName);
}

bool FB::JSAPISecureProxy::HasProperty( const std::string& propertyName )
{
    return getInnerAPI()->HasProperty(m_zone, propertyName);
}

bool FB::JSAPISecureProxy::HasProperty( int idx )
{
    return getInnerAPI()->HasProperty(m_zone, idx);
}

bool FB::JSAPISecureProxy::HasEvent( const std::string& eventName )
{
    return getInnerAPI()->HasEvent(eventName);
}

bool FB::JSAPISecureProxy::HasEvent( const std::wstring& eventName )
{
    return getInnerAPI()->HasEvent(eventName);
}

FB::variant FB::JSAPISecureProxy::GetProperty( const std::string& propertyName )
{
    return getInnerAPI()->GetProperty(m_zone, propertyName);
}

FB::variant FB::JSAPISecureProxy::GetProperty( int idx )
{
    return getInnerAPI()->GetProperty(m_zone, idx);
}

void FB::JSAPISecureProxy::SetProperty( const std::string& propertyName, const variant& value )
{
    getInnerAPI()->SetProperty(m_zone, propertyName, value);
}

void FB::JSAPISecureProxy::SetProperty( int idx, const variant& value )
{
    getInnerAPI()->SetProperty(m_zone, idx, value);
}

FB::variant FB::JSAPISecureProxy::Invoke( const std::string& methodName, const std::vector<variant>& args )
{
    return getInnerAPI()->Invoke(m_zone, methodName, args);
}
