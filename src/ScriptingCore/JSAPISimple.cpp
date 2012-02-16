/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 29, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "precompiled_headers.h" // On windows, everything above this line in PCH
#include "JSAPISimple.h"

using namespace FB;
JSAPISimple::JSAPISimple(void) : m_allowRemoveProperty(false)
{
    registerMethod( "toString", (CallMethodPtr)&JSAPISimple::callToString );
    registerMethod( "testEvent", (CallMethodPtr)&JSAPISimple::callFireEvent );
    
    registerProperty( "valid", (GetPropPtr)&JSAPISimple::getValid, NULL );

    
}

JSAPISimple::~JSAPISimple(void)
{
}


variant JSAPISimple::callFireEvent(const std::vector<FB::variant>& args_in)
{
    std::vector<FB::variant> args(args_in);
    
    try {
        std::string event = args[0].convert_cast<std::string>();
        args.erase(args.begin());
        this->FireEvent(event, args);
        return event;
    } catch (...) {
        throw invalid_arguments();
    }
}

// Example function call and read-only property; override these if desired in derived classes
variant JSAPISimple::callToString(const std::vector<FB::variant>& args)
{
    return "JSAPI Javascript Object";
}

variant JSAPISimple::getValid()
{
    return m_valid;
}

void JSAPISimple::getMemberNames(std::vector<std::string> &nameVector) const
{
    nameVector.clear();

    for (FB::MethodMap::const_iterator it = m_methodMap.begin();
        it != m_methodMap.end(); ++it) {
        nameVector.push_back(it->first);
    }
    for (FB::PropertyMap::const_iterator it = m_propertyMap.begin();
        it != m_propertyMap.end(); ++it) {
        nameVector.push_back(it->first);
    }
}

size_t JSAPISimple::getMemberCount() const
{
    return m_methodMap.size()
        + m_propertyMap.size();
}

// Methods for registering properties and functions to the auto-table
void JSAPISimple::registerMethod(const std::string& name, CallMethodPtr func)
{
    m_methodMap[name].callFunc = func;
}

void JSAPISimple::registerProperty(const std::string& name, GetPropPtr getFunc, SetPropPtr setFunc)
{
    m_propertyMap[name].getFunc = getFunc;
    m_propertyMap[name].setFunc = setFunc;
}

// Methods to query existance of members on the API
bool JSAPISimple::HasMethod(const std::string& methodName) const
{
    if (!m_valid)
        return false;

    MethodMap::const_iterator fnd = m_methodMap.find(methodName);
    if (fnd != m_methodMap.end()) {
        return true;
    } else {
        return false;
    }
}

bool JSAPISimple::HasProperty(const std::string& propertyName) const
{
    if (!m_valid)
        return false;

    PropertyMap::const_iterator fnd = m_propertyMap.find(propertyName);
    if (fnd != m_propertyMap.end()) {
        return true;
    } else {
        return false;
    }
    return false;
}


// Methods to manage properties on the API
variant JSAPISimple::GetProperty(const std::string& propertyName)
{
    if (!m_valid)
        throw object_invalidated();

    PropertyMap::const_iterator fnd = m_propertyMap.find(propertyName);
    if (fnd != m_propertyMap.end() && fnd->second.getFunc != NULL) {
        return (this->*fnd->second.getFunc)();
    } else {
        throw invalid_member(propertyName);
    }
}

void JSAPISimple::SetProperty(const std::string& propertyName, const variant& value)
{
    if (!m_valid)
        throw object_invalidated();

    PropertyMap::const_iterator fnd = m_propertyMap.find(propertyName);
    if (fnd->second.setFunc != NULL) {
        (this->*fnd->second.setFunc)(value);
    } else {
        throw invalid_member(propertyName);
    }
}

void JSAPISimple::RemoveProperty(const std::string& propertyName)
{
    if (!m_valid)
        throw object_invalidated();

    m_propertyMap.erase(propertyName);
}

bool JSAPISimple::HasProperty(int idx) const
{
    if (!m_valid)
        throw object_invalidated();

    // By default do not support indexing
    // To use array style access, override this method in your API object
    return false;
}

variant JSAPISimple::GetProperty(int idx)
{
    if (!m_valid)
        throw object_invalidated();

    // By default do not support indexing
    // To use array style access, override this method in your API object
    throw invalid_member("Array index: " + variant(idx).convert_cast<std::string>());
}

void JSAPISimple::SetProperty(int idx, const variant& value)
{
    if (!m_valid)
        throw object_invalidated();

    // By default do not support indexing
    // To use array style access, override this method in your API object
    throw invalid_member("Array index: " + variant(idx).convert_cast<std::string>());
}

void JSAPISimple::RemoveProperty(int idx)
{
    if (!m_valid)
        throw object_invalidated();

    // By default do not support indexing
    // To use array style access, override this method in your API object
    throw invalid_member("Array index: " + variant(idx).convert_cast<std::string>());
}


// Methods to manage methods on the API
variant JSAPISimple::Invoke(const std::string& methodName, const std::vector<FB::variant>& args)
{
    if (!m_valid)
        throw object_invalidated();

    MethodMap::iterator fnd = m_methodMap.find(methodName);
    if (fnd != m_methodMap.end() && fnd->second.callFunc != NULL) {
        return (this->*fnd->second.callFunc)(args);
    } else {
        throw invalid_member(methodName);
    }    
}

variant JSAPISimple::Construct(const std::vector<FB::variant>& args)
{
    if (!m_valid)
        throw object_invalidated();

    throw invalid_member("constructor");
}

