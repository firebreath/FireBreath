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

boost::shared_ptr<FB::JSAPIProxy> FB::JSAPIProxy::create( FB::JSAPIPtr &inner )
{
    // This is necessary because you can't use shared_from_this in the constructor
    boost::shared_ptr<FB::JSAPIProxy> ptr(new FB::JSAPIProxy(inner));
    inner->registerProxy(ptr);

    return ptr;
}

FB::JSAPIProxy::JSAPIProxy( FB::JSAPIPtr &inner ) : m_api(inner)
{
    if (!inner) {
        throw std::exception("Invalid inner JSAPI object passed to proxy");
    }
}

FB::JSAPIProxy::~JSAPIProxy( void )
{
}

void FB::JSAPIProxy::invalidate()
{
    m_api->invalidate();
}

void FB::JSAPIProxy::getMemberNames( std::vector<std::string> &nameVector )
{
    m_api->getMemberNames(nameVector);
}

size_t FB::JSAPIProxy::getMemberCount()
{
    return m_api->getMemberCount();
}

bool FB::JSAPIProxy::HasMethod( const std::wstring& methodName )
{
    return m_api->HasMethod(methodName);
}

bool FB::JSAPIProxy::HasMethod( const std::string& methodName )
{
    return m_api->HasMethod(methodName);
}

bool FB::JSAPIProxy::HasProperty( const std::wstring& propertyName )
{
    return m_api->HasProperty(propertyName);
}

bool FB::JSAPIProxy::HasProperty( const std::string& propertyName )
{
    return m_api->HasProperty(propertyName);
}

bool FB::JSAPIProxy::HasProperty( int idx )
{
    return m_api->HasProperty(idx);
}

bool FB::JSAPIProxy::HasEvent( const std::string& eventName )
{
    return m_api->HasEvent(eventName);
}

bool FB::JSAPIProxy::HasEvent( const std::wstring& eventName )
{
    return m_api->HasEvent(eventName);
}

FB::variant FB::JSAPIProxy::GetProperty( const std::wstring& propertyName )
{
    return m_api->GetProperty(propertyName);
}

FB::variant FB::JSAPIProxy::GetProperty( const std::string& propertyName )
{
    return m_api->GetProperty(propertyName);
}

FB::variant FB::JSAPIProxy::GetProperty( int idx )
{
    return m_api->GetProperty(idx);
}

void FB::JSAPIProxy::SetProperty( const std::wstring& propertyName, const variant& value )
{
    m_api->SetProperty(propertyName, value);
}

void FB::JSAPIProxy::SetProperty( const std::string& propertyName, const variant& value )
{
    m_api->SetProperty(propertyName, value);
}

void FB::JSAPIProxy::SetProperty( int idx, const variant& value )
{
    m_api->SetProperty(idx, value);
}

FB::variant FB::JSAPIProxy::Invoke( const std::wstring& methodName, const std::vector<variant>& args )
{
    return m_api->Invoke(methodName, args);
}

FB::variant FB::JSAPIProxy::Invoke( const std::string& methodName, const std::vector<variant>& args )
{
    return m_api->Invoke(methodName, args);
}
