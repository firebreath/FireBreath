/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    Dec 22, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#ifndef H_FAKE_JS_MAP
#define H_FAKE_JS_MAP

#include "JSObject.h"
#include <iterator>

class FakeJsMap : public FB::JSObject
{
    typedef std::vector<std::string> StringVec;
    typedef FB::VariantMap::value_type OutPair;

    struct GrabKeys {
        std::back_insert_iterator<StringVec> inserter;
        GrabKeys(StringVec& keys) : inserter(std::back_inserter(keys)) {}
        void operator()(const OutPair& p) {
            *inserter++ = p.first;
        }
    };
public:
    FakeJsMap(const FB::VariantMap& values)
      : FB::JSObject(FB::BrowserHostPtr()), m_values(values)
    {
        std::for_each(m_values.begin(), m_values.end(), GrabKeys(m_names));
    }

    FB::JSAPIPtr getJSAPI() const { return FB::JSAPIPtr(); }
    void invalidate() { m_values.clear(); }
    bool isValid() { return true; }
    
    bool HasMethod(const std::string&) const { return false; }
    void SetProperty(int, const FB::variant&) {}
    void SetProperty(const std::string&, const FB::variant&) {}
    FB::variant Invoke(const std::string&, const FB::VariantList&) { return FB::variant(); }
    FB::JSObjectPtr Construct(const std::string&, const FB::VariantList&) { return FB::JSObjectPtr(); }

    // Methods for enumeration
    virtual void getMemberNames(StringVec &names) const
    {
        names = m_names;
    }
    
    virtual size_t getMemberCount() const
    {
        return m_values.size(); 
    }


    bool HasProperty(const std::string& s) const
    { 
        return (s == "length"); 
    }
    
    bool HasProperty(int index) const
    { 
        return ((unsigned)index < m_values.size()); 
    }
    
    FB::variant GetProperty(const std::string& s) 
    { 
        if(s == "length")
            return (int)m_values.size();
        FB::VariantMap::const_iterator it = m_values.find(s);
        if(it != m_values.end())
            return it->second;
        throw FB::script_error(std::string("no such property '")+s+"'");
    }
    
    FB::variant GetProperty(int index)     
    { 
        if((unsigned)index >= m_values.size()) {
            std::ostringstream ss;
            ss << "index out of range - got " << index << ", size is " << m_values.size();
            throw FB::script_error(ss.str());
        }
        return m_values[m_names[index]];
    }

    void RemoveProperty(const std::string& propertyName) {
        FB::VariantMap::iterator it = m_values.find(propertyName);
        if(it != m_values.end())
            m_values.erase(it);
    }
    
    void RemoveProperty(int idx)
    {
        throw FB::script_error(std::string("Cannot remove property '") + boost::lexical_cast<std::string>(idx) + "'");
    }
    
    FB::variant Construct(const std::vector<FB::variant>& args) { throw FB::script_error("Constructor is not implemented"); }
private:
    FB::VariantMap m_values;
    StringVec m_names;
};

#endif

