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
#include "utf8_tools.h"

FB::JSAPIAuto::JSAPIAuto(const std::string& description)
  : m_methodFunctorMap(),
    m_propertyFunctorsMap(),
    m_description(description),
    m_allowDynamicAttributes(true)
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
    return m_description;
}

bool FB::JSAPIAuto::get_valid()
{
    return true;
}

void FB::JSAPIAuto::registerMethod(const std::wstring& name, const CallMethodFunctor& func)
{
    registerMethod(FB::wstring_to_utf8(name), func);
}

void FB::JSAPIAuto::registerMethod(const std::string& name, const CallMethodFunctor& func)
{
    m_methodFunctorMap[name] = func;
}

void FB::JSAPIAuto::registerProperty(const std::wstring& name, const PropertyFunctors& func)
{
    registerProperty(FB::wstring_to_utf8(name), func);
}

void FB::JSAPIAuto::registerProperty(const std::string& name, const PropertyFunctors& propFuncs)
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

bool FB::JSAPIAuto::HasMethod(const std::string& methodName)
{
    if(!m_valid)
        return false;

    return (m_methodFunctorMap.find(methodName) != m_methodFunctorMap.end());
}

bool FB::JSAPIAuto::HasProperty(const std::string& propertyName)
{
    if(!m_valid)
        return false;

    // To be able to set dynamic properties, we have to respond true always
    if (m_allowDynamicAttributes && !HasMethod(propertyName))
        return true;

    return m_propertyFunctorsMap.find(propertyName) != m_propertyFunctorsMap.end() || m_attributes.find(propertyName) != m_attributes.end();
}

bool FB::JSAPIAuto::HasProperty(int idx)
{
    if(!m_valid)
        return false;

    // To be able to set dynamic properties, we have to respond true always
    if (m_allowDynamicAttributes)
        return true;

    return m_attributes.find(boost::lexical_cast<std::string>(idx)) != m_attributes.end();
}

FB::variant FB::JSAPIAuto::GetProperty(const std::string& propertyName)
{
    if(!m_valid)
        throw object_invalidated();

    PropertyFunctorsMap::const_iterator it = m_propertyFunctorsMap.find(propertyName);
    if(it != m_propertyFunctorsMap.end())
        return it->second.get();
    else {
        AttributeMap::iterator fnd = m_attributes.find(propertyName);
        if (fnd != m_attributes.end())
            return fnd->second.value;
        else if (m_allowDynamicAttributes) {
            return FB::FBVoid(); // If we allow dynamic attributes then we need to
                                 // return void if the property doesn't exist;
                                 // otherwise checking a property will throw an exception
        } else {
            throw invalid_member(propertyName);
        }
    }
}

void FB::JSAPIAuto::SetProperty(const std::string& propertyName, const variant& value)
{
    if(!m_valid)
        throw object_invalidated();

    PropertyFunctorsMap::iterator it = m_propertyFunctorsMap.find(propertyName);
    if(it != m_propertyFunctorsMap.end()) {
        try {
            it->second.set(value);
        } catch (const FB::bad_variant_cast& ex) {
            std::string errorMsg("Could not convert from ");
            errorMsg += ex.from;
            errorMsg += " to ";
            errorMsg += ex.to;
            throw FB::invalid_arguments(errorMsg);
        }
    } else if (m_allowDynamicAttributes || (m_attributes.find(propertyName) != m_attributes.end() && !m_attributes[propertyName].readonly)) {
        registerAttribute(propertyName, value);
    } else {
        throw invalid_member(propertyName);
    }
}

FB::variant FB::JSAPIAuto::GetProperty(int idx)
{
    if(!m_valid)
        throw object_invalidated();

    std::string id = boost::lexical_cast<std::string>(idx);
    AttributeMap::iterator fnd = m_attributes.find(id);
    if (fnd != m_attributes.end())
        return fnd->second.value;
    else if (m_allowDynamicAttributes) {
        return FB::FBVoid(); // If we allow dynamic attributes then we need to
                             // return void if the property doesn't exist;
                             // otherwise checking a property will throw an exception
    } else {
        throw invalid_member(boost::lexical_cast<std::string>(idx));
    }

    // This method should be overridden to access properties in an array style from javascript,
    // i.e. var value = pluginObj[45]; would call GetProperty(45)
    // Default is to throw "invalid member" unless m_attributes has something matching
}

void FB::JSAPIAuto::SetProperty(int idx, const variant& value)
{
    std::string id(boost::lexical_cast<std::string>(idx));
    if (m_allowDynamicAttributes || (m_attributes.find(id) != m_attributes.end() && !m_attributes[id].readonly)) {
        registerAttribute(id, value);
    } else {
        throw invalid_member(FB::variant(idx).convert_cast<std::string>());
    }
}

FB::variant FB::JSAPIAuto::Invoke(const std::string& methodName, const std::vector<variant> &args)
{
    if(!m_valid)
        throw object_invalidated();

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
}

void FB::JSAPIAuto::registerAttribute( const std::string &name, const FB::variant& value, bool readonly /*= false*/ )
{
    Attribute attr = {value, readonly};
    m_attributes[name] = attr;
}
