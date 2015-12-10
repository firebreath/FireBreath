
/**********************************************************\
 Original Author: Georg Fritzsche
 
 Created:    Nov 7, 2010
 License:    Dual license model; choose one of two:
 New BSD License
 http://www.opensource.org/licenses/bsd-license.php
 - or -
 GNU Lesser General Public License, version 2.1
 http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 20100 Georg Fritzsche, Firebreath development team
 \**********************************************************/

#pragma once
#ifndef FB_H_CComVariantUTIL
#define FB_H_CComVariantUTIL

#include <map>
#include <string>
#include <typeinfo>
#include <memory>
#include <mutex>

#include <type_traits>

#include "FactoryBase.h"
#include "ActiveXBrowserHost.h"
#include "DOM/Document.h"
#include "DOM/Window.h"
#include "AXDOM/Window.h"
#include "AXDOM/Document.h"
#include "AXDOM/Element.h"
#include "AXDOM/Node.h"
#include <atlsafe.h>

namespace FB { namespace ActiveX
{    
    struct type_info_less
    {
        bool operator() (const std::type_info* const lhs, const std::type_info* const rhs) const
        {
            return lhs->before(*rhs) ? true : false;
        }
    };
    
    using ComVariantBuilder = CComVariant(*)(const ActiveXBrowserHostPtr&, const FB::variant&);
    using ComVariantBuilderMap = std::map<std::type_info const*, ComVariantBuilder, type_info_less>;
    
    namespace {
        ComVariantBuilderMap comVariantBuilderMap;
        std::once_flag init_flag;
    }
    
    //  GJS  ---
    //  I would probably put the ComVariantBuilderMap code into ComVariantUtil.cpp?
    template<class T>
    inline ComVariantBuilderMap::value_type makeBuilderEntry()
    {
        return ComVariantBuilderMap::value_type(&typeid(T), select_ccomvariant_builder::select<T>());
    }
    
    inline void makeComVariantBuilderMap(ComVariantBuilderMap& tdm)
    {
        tdm.insert(makeBuilderEntry<bool>());
        tdm.insert(makeBuilderEntry<char>());
        tdm.insert(makeBuilderEntry<unsigned char>());
        tdm.insert(makeBuilderEntry<short>());
        tdm.insert(makeBuilderEntry<unsigned short>());
        tdm.insert(makeBuilderEntry<int>());
        tdm.insert(makeBuilderEntry<unsigned int>());
        tdm.insert(makeBuilderEntry<long>());
        tdm.insert(makeBuilderEntry<unsigned long>());
        
#ifndef BOOST_NO_LONG_LONG
        tdm.insert(makeBuilderEntry<long long>());
        tdm.insert(makeBuilderEntry<unsigned long long>());
#endif
        
        tdm.insert(makeBuilderEntry<float>());
        tdm.insert(makeBuilderEntry<double>());
        
        tdm.insert(makeBuilderEntry<std::string>());
        tdm.insert(makeBuilderEntry<std::wstring>());
        
        tdm.insert(makeBuilderEntry<FB::VariantList>());
        tdm.insert(makeBuilderEntry<FB::VariantMap>());
        tdm.insert(makeBuilderEntry<FB::JSAPIPtr>());
        tdm.insert(makeBuilderEntry<FB::JSAPIWeakPtr>());
        tdm.insert(makeBuilderEntry<FB::JSObjectPtr>());
        
        tdm.insert(makeBuilderEntry<FB::FBVoid>());
        tdm.insert(makeBuilderEntry<FB::FBNull>());
        tdm.insert(makeBuilderEntry<const std::exception>());
        tdm.insert(makeBuilderEntry<const std::exception_ptr>());
    }
    
    inline const ComVariantBuilderMap& getComVariantBuilderMap()
    {
        // Thread safety is required because IE10+ can run
        // plug-ins in multiple threads within a single process.
        std::call_once(init_flag, std::bind(&makeComVariantBuilderMap, std::ref(comVariantBuilderMap)));
        
        return comVariantBuilderMap;
    }
    //  GJS  ---
    
    template<class T>
    inline CComVariant makeArithmeticComVariant(const ActiveXBrowserHostPtr& host, const FB::variant& var)
    {
        return var.convert_cast<T>();
    }
    
    template<> inline
    CComVariant makeArithmeticComVariant<char>(const ActiveXBrowserHostPtr& host, const FB::variant& var)
    {
        return var.cast<char>();
    }
    
    template<> inline
    CComVariant makeArithmeticComVariant<unsigned char>(const ActiveXBrowserHostPtr& host, const FB::variant& var)
    {
        return var.cast<unsigned char>();
    }
    
    template<class T> 
    CComVariant makeComVariant(const ActiveXBrowserHostPtr& host, const FB::variant& var)
    {
        CComVariant out;
        out.ChangeType(VT_NULL);
        return out;
    }
    
    template<> inline
    CComVariant makeComVariant<FB::FBNull>(const ActiveXBrowserHostPtr& host, const FB::variant& var)
    {
        CComVariant out;
        out.ChangeType(VT_NULL);
        return out;
    }
    
    template<> inline
    CComVariant makeComVariant<FB::FBVoid>(const ActiveXBrowserHostPtr& host, const FB::variant& var)
    {
        CComVariant out; // Just leave it at VT_EMPTY
        return out;
    }
    
    template<> inline
    CComVariant makeComVariant<std::string>(const ActiveXBrowserHostPtr& host, const FB::variant& var)
    {
        std::wstring wstr = var.convert_cast<std::wstring>();
        CComBSTR bStr(wstr.c_str());
        return bStr;
    }
    
    template<> inline
    CComVariant makeComVariant<std::wstring>(const ActiveXBrowserHostPtr& host, const FB::variant& var)
    {
        std::wstring wstr = var.convert_cast<std::wstring>();
        CComBSTR bStr(wstr.c_str());
        return bStr;
    }
    
    template<> inline
    CComVariant makeComVariant<FB::VariantList>(const ActiveXBrowserHostPtr& host, const FB::variant& var)
    {
        //  GJS  ---
        CComVariant outVar;
        FB::VariantList inArr = var.cast<FB::VariantList>();
        if (host->hasHTMLWindow())
        {
            IDispatchAPIPtr win = std::dynamic_pointer_cast<IDispatchAPI>(host->getDOMWindow()->getJSObject());
            IDispatchAPIPtr api = std::dynamic_pointer_cast<IDispatchAPI>(win->InvokeSync("Array", inArr).cast<JSObjectPtr>());
            if (api) {
                return api->getIDispatch();
            } 
        }
        else
        {
            CComSafeArray<VARIANT> sa;
            sa.Create();
            const ComVariantBuilderMap& builderMap = getComVariantBuilderMap();
            for (FB::VariantList::iterator itr = inArr.begin(); itr != inArr.end(); ++itr) {
                const std::type_info& type = itr->get_type();
                ComVariantBuilderMap::const_iterator found = builderMap.find(&type);
                
                if (found == builderMap.end())
                    continue;
                    
                CComVariant var = (found->second)(host, *itr);
                sa.Add(var);
            }
            
            outVar = sa.Detach();
        }
        return outVar;
        //  GJS  ---
    }
    
    template<> inline
    CComVariant makeComVariant<FB::VariantMap>(const ActiveXBrowserHostPtr& host, const FB::variant& var)
    {
        //  GJS  ---
        CComVariant outVar;
        FB::VariantMap inMap = var.cast<FB::VariantMap>();
        if (host->hasHTMLWindow())
        {
            IDispatchAPIPtr win = std::dynamic_pointer_cast<IDispatchAPI>(host->getDOMWindow()->getJSObject());
            IDispatchAPIPtr api = std::dynamic_pointer_cast<IDispatchAPI>(win->InvokeSync("Object", FB::VariantList{}).cast<JSObjectPtr>());
            for (FB::VariantMap::iterator it = inMap.begin(); it != inMap.end(); ++it) {
                api->SetProperty(it->first, it->second);
            }
            if (api) {
                outVar = api->getIDispatch();
            }
        }
        else
        {
            CComSafeArray<VARIANT> sa;
            sa.Create();
            const ComVariantBuilderMap& builderMap = getComVariantBuilderMap();
            for (FB::VariantMap::iterator itr = inMap.begin(); itr != inMap.end(); ++itr) {
                const std::type_info& valType = itr->second.get_type();
                ComVariantBuilderMap::const_iterator valTypeFound = builderMap.find(&valType);
                if (valTypeFound == builderMap.end())
                    continue;
                    
                CComSafeArray<VARIANT> sa2;
                CComVariant key = makeComVariant<std::string>(host, itr->first);
                CComVariant val = (valTypeFound->second)(host, itr->second);
                sa2.Add(key);
                sa2.Add(val);
                sa.Add(sa2.Detach());
            }
                outVar = sa.Detach();
        }
        return outVar;
        //  GJS  ---
    }
    
    template<> inline
    CComVariant makeComVariant<FB::JSAPIPtr>(const ActiveXBrowserHostPtr& host, const FB::variant& var)
    {
        CComVariant outVar;
        
        FB::JSAPIPtr obj(var.cast<FB::JSAPIPtr>());
        IDispatchAPIPtr api = std::dynamic_pointer_cast<IDispatchAPI>(obj);
        if (api) {
            outVar = api->getIDispatch();
        } else {
            if (obj) {
                // Add obj to the list of shared_ptrs that browserhost keeps
                outVar = host->getJSAPIWrapper(obj, true);
                outVar.pdispVal->Release();
            } else {
                outVar.ChangeType(VT_NULL);
            }
        }
        
        return outVar;
    }
    
    template<> inline
    CComVariant makeComVariant<FB::JSAPIWeakPtr>(const ActiveXBrowserHostPtr& host, const FB::variant& var)
    {
        CComVariant outVar;
        
        FB::JSAPIPtr obj(var.convert_cast<FB::JSAPIPtr>());
        IDispatchAPIPtr api = std::dynamic_pointer_cast<IDispatchAPI>(obj);
        if (api) {
            outVar = api->getIDispatch();
        } else {
            if (obj) {
                outVar = host->getJSAPIWrapper(obj, false);
                outVar.pdispVal->Release();
            } else {
                outVar.ChangeType(VT_NULL);
            }
        }
        
        return outVar;
    }
    
    template<> inline
    CComVariant makeComVariant<FB::JSObjectPtr>(const ActiveXBrowserHostPtr& host, const FB::variant& var)
    {
        CComVariant outVar;
        
        FB::JSObjectPtr obj(var.cast<FB::JSObjectPtr>());
        IDispatchAPIPtr api = std::dynamic_pointer_cast<IDispatchAPI>(obj);
        if (api) {
            outVar = api->getIDispatch();
        } else {
            if (obj) {
                FB::JSAPIPtr ptr(var.convert_cast<FB::JSAPIPtr>());
                outVar = getFactoryInstance()->createCOMJSObject(host, ptr, true);
                outVar.pdispVal->Release();
            } else {
                outVar.ChangeType(VT_NULL);
            }
        }
        
        return outVar;
    }
    
    template<> inline
    CComVariant makeComVariant<const std::exception>(const ActiveXBrowserHostPtr& host, const FB::variant& var)
    {
        const std::exception e = var.cast<const std::exception>();
        std::wstring wstr = FB::utf8_to_wstring(e.what());
        CComBSTR bStr(wstr.c_str());
        return bStr;
    }
    
    template<> inline
    CComVariant makeComVariant<const std::exception_ptr>(const ActiveXBrowserHostPtr& host, const FB::variant& var)
    {
        const std::exception_ptr ep = var.cast<const std::exception_ptr>();
        if(ep) {
            try {
                std::rethrow_exception(ep);
            } catch(const std::exception& e) {
                std::wstring wstr = FB::utf8_to_wstring(e.what());
                CComBSTR bStr(wstr.c_str());
                return bStr;
            } catch(...) {}
        }
        std::wstring wstr(L"Unknown exception");
        CComBSTR bStr(wstr.c_str());
        return bStr;
    }
    
    namespace select_ccomvariant_builder
    {        
        template<class T>
        ComVariantBuilder isArithmetic(const std::true_type& /* is_arithmetic */)
        {
            return &makeArithmeticComVariant<T>;
        }
        
        template<class T>
        ComVariantBuilder isArithmetic(const std::false_type& /* is_arithmetic */)
        {
            return &makeComVariant<T>;
        }
        
        template<class T>
        ComVariantBuilder select()
        {
            return isArithmetic<T>(std::is_arithmetic<T>());
        }
    }
} }

#endif
