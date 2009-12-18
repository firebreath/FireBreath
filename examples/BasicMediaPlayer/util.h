
#ifndef BMP_UTIL_H
#define BMP_UTIL_H

#include "APITypes.h"
#include "BrowserObjectAPI.h"
#include "variant_list.h"

class ArrayHelper : public FB::BrowserObjectAPI
{
public:
    ArrayHelper(FB::BrowserHostWrapper* host) 
      : FB::BrowserObjectAPI(host), 
        m_values() 
    {}
    ArrayHelper(const FB::VariantList& values, FB::BrowserHostWrapper* host)
      : FB::BrowserObjectAPI(host),
        m_values(values) 
    {}

    // Methods to query existance of members on the API
    virtual bool HasMethod(std::string name) 
    { 
        return (name == "toString"); 
    }
    
    virtual bool HasProperty(std::string name)
    {
        return (name == "length"); 
    }
    
    virtual bool HasProperty(int idx) 
    {
        return (idx < (int)m_values.size()); 
    }
    
    virtual bool HasEvent(std::string name) 
    {
        return false; 
    }

    // Methods to manage properties on the API
    virtual FB::variant GetProperty(std::string propertyName) 
    {
        return m_values.size(); 
    }
    
    virtual void SetProperty(std::string propertyName, const FB::variant value) 
    {
    
    }
    
    virtual FB::variant GetProperty(int idx) 
    {
        return m_values[idx]; 
    }
    
    virtual void SetProperty(int idx, const FB::variant value) 
    {
    
    }

    // Methods to manage methods on the API
    virtual FB::variant Invoke(std::string methodName, std::vector<FB::variant>& args) 
    {
        if(methodName == "toString")
        {
            typedef std::vector<std::string> StringVec;
            StringVec v = FB::convert_variant_list<StringVec>(m_values);
            std::stringstream ss;
            for(StringVec::iterator it = v.begin(); it != v.end(); ++it)
                ss << *it << '\n';
            return ss.str();
        }

        return FB::variant();
    }

private:
    FB::VariantList m_values;
};

#endif