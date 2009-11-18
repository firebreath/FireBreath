/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    November 7, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#include "JSAPIAuto.h"

FB::JSAPIAuto::JSAPIAuto()
  : m_methodFunctorMap(),
    m_propertyFunctorsMap()
{

}

FB::JSAPIAuto::~JSAPIAuto()
{

}

void FB::JSAPIAuto::registerMethod(const std::string& name, CallMethodFunctor func)
{
    m_methodFunctorMap[name] = func;
}

void FB::JSAPIAuto::registerProperty(const std::string& name, PropertyFunctors propFuncs)
{
    m_propertyFunctorsMap[name] = propFuncs;
}

bool FB::JSAPIAuto::HasMethod(std::string methodName)
{
    if(!m_valid)
        return false;

    return (m_methodFunctorMap.find(methodName) != m_methodFunctorMap.end());
}

bool FB::JSAPIAuto::HasProperty(std::string propertyName)
{
    if(!m_valid)
        return false;

    return (m_propertyFunctorsMap.find(propertyName) != m_propertyFunctorsMap.end());
}

bool FB::JSAPIAuto::HasProperty(int idx)
{
    if(!m_valid)
        return false;

    // By default, we don't have any indexed properties; so return false.  To add indexed
    // properties, override this method
    return false;
}

FB::variant FB::JSAPIAuto::GetProperty(std::string propertyName)
{
    if(!m_valid)
        throw object_invalidated();

    PropertyFunctorsMap::const_iterator it = m_propertyFunctorsMap.find(propertyName);
    if(it == m_propertyFunctorsMap.end())
        throw invalid_member(propertyName);
    
    return it->second.get();
}

void FB::JSAPIAuto::SetProperty(std::string propertyName, const variant value)
{
    if(!m_valid)
        throw object_invalidated();

    PropertyFunctorsMap::iterator it = m_propertyFunctorsMap.find(propertyName);
    if(it == m_propertyFunctorsMap.end())
        throw invalid_member(propertyName);
    
    it->second.set(value);
}

FB::variant FB::JSAPIAuto::GetProperty(int idx)
{
    if(!m_valid)
        throw object_invalidated();

    // This method should be overridden to access properties in an array style from javascript,
    // i.e. var value = pluginObj[45]; would call GetProperty(45)
    // Default is to throw "invalid member"
    // Incidently, this isn't a very efficient way to convert this to a string; but, it shouldn't
    // get called much, and I'm lazy (taxilian)
    throw invalid_member(FB::variant(idx).convert_cast<std::string>());
}

void FB::JSAPIAuto::SetProperty(int idx, const variant value)
{
    throw invalid_member(FB::variant(idx).convert_cast<std::string>());
}

FB::variant FB::JSAPIAuto::Invoke(std::string methodName, std::vector<variant> &args)
{
    if(!m_valid)
        throw object_invalidated();

    MethodFunctorMap::iterator it = m_methodFunctorMap.find(methodName);
    if(it == m_methodFunctorMap.end())
        throw invalid_member(methodName);

    return it->second(args);
}
