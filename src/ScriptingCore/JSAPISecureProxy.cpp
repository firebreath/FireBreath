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

#include "JSAPISecureProxy.h"

boost::shared_ptr<FB::JSAPISecureProxy> FB::JSAPISecureProxy::create( const FB::SecurityZone zone, const FB::JSAPIPtr &inner )
{
    boost::shared_ptr<FB::JSAPISecureProxy> ptr(new FB::JSAPISecureProxy(zone, inner));
    inner->registerProxy(ptr);
    // This is necessary because you can't use shared_from_this in the constructor
    return ptr;
}

FB::JSAPISecureProxy::JSAPISecureProxy( const FB::SecurityZone zone, const FB::JSAPIPtr &inner ) : m_zone(zone), m_api(FB::ptr_cast<FB::JSAPISecure>(inner))
{
    if (!m_api) {
        throw std::exception("Invalid inner JSAPISecure object passed to proxy");
    }
}

FB::JSAPISecureProxy::~JSAPISecureProxy( void )
{
}

void FB::JSAPISecureProxy::invalidate()
{
    m_api->invalidate();
}

void FB::JSAPISecureProxy::getMemberNames( std::vector<std::string> &nameVector )
{
    m_api->getMemberNames(nameVector);
}

size_t FB::JSAPISecureProxy::getMemberCount()
{
    return m_api->getMemberCount();
}

bool FB::JSAPISecureProxy::HasMethod( const std::string& methodName )
{
    return m_api->HasMethod(m_zone, methodName);
}

bool FB::JSAPISecureProxy::HasProperty( const std::string& propertyName )
{
    return m_api->HasProperty(m_zone, propertyName);
}

bool FB::JSAPISecureProxy::HasProperty( int idx )
{
    return m_api->HasProperty(m_zone, idx);
}

bool FB::JSAPISecureProxy::HasEvent( const std::string& eventName )
{
    return m_api->HasEvent(eventName);
}

bool FB::JSAPISecureProxy::HasEvent( const std::wstring& eventName )
{
    return m_api->HasEvent(eventName);
}

FB::variant FB::JSAPISecureProxy::GetProperty( const std::string& propertyName )
{
    return m_api->GetProperty(m_zone, propertyName);
}

FB::variant FB::JSAPISecureProxy::GetProperty( int idx )
{
    return m_api->GetProperty(m_zone, idx);
}

void FB::JSAPISecureProxy::SetProperty( const std::string& propertyName, const variant& value )
{
    m_api->SetProperty(m_zone, propertyName, value);
}

void FB::JSAPISecureProxy::SetProperty( int idx, const variant& value )
{
    m_api->SetProperty(m_zone, idx, value);
}

FB::variant FB::JSAPISecureProxy::Invoke( const std::string& methodName, const std::vector<variant>& args )
{
    return m_api->Invoke(m_zone, methodName, args);
}
