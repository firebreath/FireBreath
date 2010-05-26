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

#include "JSAPIAuto.h"

FB::JSAPIAuto::JSAPIAuto()
  : m_methodFunctorMap(),
    m_propertyFunctorsMap()
{
    registerMethod("toString",  make_method(this, &JSAPIAuto::ToString));
    
    registerProperty("value", make_property(this, &JSAPIAuto::ToString));
    registerProperty("valid", make_property(this, &JSAPIAuto::get_valid));
}

FB::JSAPIAuto::~JSAPIAuto()
{

}

std::string FB::JSAPIAuto::ToString()
{
    return "<JSAPI-Auto driven Javascript Object>";
}

bool FB::JSAPIAuto::get_valid()
{
    return true;
}

void FB::JSAPIAuto::registerMethod(const std::string& name, CallMethodFunctor func)
{
    m_methodFunctorMap[name] = func;
}

void FB::JSAPIAuto::registerProperty(const std::string& name, PropertyFunctors propFuncs)
{
    m_propertyFunctorsMap[name] = propFuncs;
}

void FB::JSAPIAuto::getMemberNames(std::vector<std::string> &nameVector)
{
    nameVector.clear();

    for (FB::MethodFunctorMap::iterator it = m_methodFunctorMap.begin();
        it != m_methodFunctorMap.end(); it++) {
        nameVector.push_back(it->first);
    }
    for (FB::PropertyFunctorsMap::iterator it = m_propertyFunctorsMap.begin();
        it != m_propertyFunctorsMap.end(); it++) {
        nameVector.push_back(it->first);
    }
    for (FB::EventSingleMap::iterator it = m_defEventMap.begin();
        it != m_defEventMap.end(); it++) {
        nameVector.push_back(it->first);
    }
}

size_t FB::JSAPIAuto::getMemberCount()
{
    return m_methodFunctorMap.size()
        + m_propertyFunctorsMap.size()
        + m_defEventMap.size();
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
    
    try {
        it->second.set(value);
    } catch (const FB::bad_variant_cast& ex) {
        std::string errorMsg("Could not convert from ");
        errorMsg += ex.from;
        errorMsg += " to ";
        errorMsg += ex.to;
        throw FB::invalid_arguments(errorMsg);
    }}

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

    try {
        MethodFunctorMap::iterator it = m_methodFunctorMap.find(methodName);
        if(it == m_methodFunctorMap.end())
            throw invalid_member(methodName);

        return it->second(args);
    } catch (const FB::bad_variant_cast& ex) {
        std::string errorMsg("Could not convert from ");
        errorMsg += ex.from;
        errorMsg += " to ";
        errorMsg += ex.to;
        throw FB::invalid_arguments(errorMsg);
    }
}
