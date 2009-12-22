/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    Dec 16, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#ifndef H_FAKE_JS_ARRAY
#define H_FAKE_JS_ARRAY

#include "BrowserObjectAPI.h"

class FakeJsArray : public FB::BrowserObjectAPI
{
public:
    FakeJsArray(const FB::VariantList& values)
      : m_values(values), FB::BrowserObjectAPI(0)
    {
    }

    bool HasMethod(std::string) { return false; }
    void FB::JSAPI::SetProperty(int,const FB::variant) {}
    void FB::JSAPI::SetProperty(std::string,const FB::variant) {}
    FB::variant FB::JSAPI::Invoke(std::string,FB::VariantList&) { return FB::variant(); }

    // Methods for enumeration
    virtual void getMemberNames(std::vector<std::string> &nameVector) { }
    virtual size_t getMemberCount() { return 0; }


    bool HasProperty(std::string s)    
    { 
        return (s == "length"); 
    }
    
    bool HasProperty(int index)        
    { 
        return ((unsigned)index < m_values.size()); 
    }
    
    FB::variant GetProperty(std::string s) 
    { 
        if(s == "length")
            return (int)m_values.size();
        throw FB::script_error(std::string("no such property '")+s+"'");
    }
    
    FB::variant GetProperty(int index)     
    { 
        if((unsigned)index >= m_values.size()) {
            std::stringstream ss;
            ss << "index out of range - got " << index << ", size is " << m_values.size();
            throw FB::script_error(ss.str());
        }
        return m_values[index]; 
    }

private:
    FB::VariantList m_values;
};

#endif