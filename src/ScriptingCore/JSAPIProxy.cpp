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

boost::shared_ptr<FB::JSAPIProxy> FB::JSAPIProxy::create( FB::JSAPIWeakPtr &inner )
{
    // This is necessary because you can't use shared_from_this in the constructor
    boost::shared_ptr<FB::JSAPIProxy> ptr(new FB::JSAPIProxy(inner));
    inner.lock()->registerProxy(ptr);

    return ptr;
}

FB::JSAPIProxy::JSAPIProxy( FB::JSAPIPtr &inner ) : m_api(inner), m_apiWeak(inner)
{
    if (!inner) {
        throw FB::script_error("Invalid inner JSAPI object passed to proxy");
    }
}

FB::JSAPIProxy::JSAPIProxy( FB::JSAPIWeakPtr &inner ) : m_apiWeak(inner)
{

}

FB::JSAPIProxy::~JSAPIProxy( void )
{
}

void FB::JSAPIProxy::invalidate()
{
    getAPI()->invalidate();
}

void FB::JSAPIProxy::getMemberNames( std::vector<std::string> &nameVector )
{
    getAPI()->getMemberNames(nameVector);
}

size_t FB::JSAPIProxy::getMemberCount()
{
    return getAPI()->getMemberCount();
}

bool FB::JSAPIProxy::HasMethod( const std::wstring& methodName )
{
    return getAPI()->HasMethod(methodName);
}

bool FB::JSAPIProxy::HasMethod( const std::string& methodName )
{
    return getAPI()->HasMethod(methodName);
}

bool FB::JSAPIProxy::HasProperty( const std::wstring& propertyName )
{
    return getAPI()->HasProperty(propertyName);
}

bool FB::JSAPIProxy::HasProperty( const std::string& propertyName )
{
    return getAPI()->HasProperty(propertyName);
}

bool FB::JSAPIProxy::HasProperty( int idx )
{
    return getAPI()->HasProperty(idx);
}

bool FB::JSAPIProxy::HasEvent( const std::string& eventName )
{
    return getAPI()->HasEvent(eventName);
}

bool FB::JSAPIProxy::HasEvent( const std::wstring& eventName )
{
    return getAPI()->HasEvent(eventName);
}

FB::variant FB::JSAPIProxy::GetProperty( const std::wstring& propertyName )
{
    return getAPI()->GetProperty(propertyName);
}

FB::variant FB::JSAPIProxy::GetProperty( const std::string& propertyName )
{
    return getAPI()->GetProperty(propertyName);
}

FB::variant FB::JSAPIProxy::GetProperty( int idx )
{
    return getAPI()->GetProperty(idx);
}

void FB::JSAPIProxy::SetProperty( const std::wstring& propertyName, const variant& value )
{
    getAPI()->SetProperty(propertyName, value);
}

void FB::JSAPIProxy::SetProperty( const std::string& propertyName, const variant& value )
{
    getAPI()->SetProperty(propertyName, value);
}

void FB::JSAPIProxy::SetProperty( int idx, const variant& value )
{
    getAPI()->SetProperty(idx, value);
}

FB::variant FB::JSAPIProxy::Invoke( const std::wstring& methodName, const std::vector<variant>& args )
{
    return getAPI()->Invoke(methodName, args);
}

FB::variant FB::JSAPIProxy::Invoke( const std::string& methodName, const std::vector<variant>& args )
{
    return getAPI()->Invoke(methodName, args);
}
