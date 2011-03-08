
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

#include <boost/assign/list_of.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/integer_traits.hpp>

#include "FactoryBase.h"
#include "ActiveXBrowserHost.h"
#include "DOM/Document.h"
#include "DOM/Window.h"
#include "AXDOM/Window.h"
#include "AXDOM/Document.h"
#include "AXDOM/Element.h"
#include "AXDOM/Node.h"

namespace FB { namespace ActiveX
{    
    struct type_info_less
    {
        bool operator() (const std::type_info* const lhs, const std::type_info* const rhs) const
        {
            return lhs->before(*rhs) ? true : false;
        }
    };
    
    typedef CComVariant (*ComVariantBuilder)(const ActiveXBrowserHostPtr&, const FB::variant&);    
    typedef std::map<std::type_info const*, ComVariantBuilder, type_info_less> ComVariantBuilderMap;
    
    template<class T>
    CComVariant makeArithmeticComVariant(const ActiveXBrowserHostPtr& host, const FB::variant& var)
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
        CComVariant outVar;

        FB::JSObjectPtr outArr = host->getDOMWindow()->createArray();
        FB::VariantList inArr = var.cast<FB::VariantList>();
        for (FB::VariantList::iterator it = inArr.begin(); it != inArr.end(); it++) {
            FB::VariantList vl = boost::assign::list_of(*it);
            outArr->Invoke("push", vl);
        }
        IDispatchAPIPtr api = ptr_cast<IDispatchAPI>(outArr);
        if (api) {
            return api->getIDispatch();
        } 

        return outVar;
    }
    
    template<> inline
    CComVariant makeComVariant<FB::VariantMap>(const ActiveXBrowserHostPtr& host, const FB::variant& var)
    {
        CComVariant outVar;

        FB::JSObjectPtr out = host->getDOMWindow()->createMap();
        FB::VariantMap inMap = var.cast<FB::VariantMap>();
        for (FB::VariantMap::iterator it = inMap.begin(); it != inMap.end(); it++) {
            out->SetProperty(it->first, it->second);
        }
        IDispatchAPIPtr api = ptr_cast<IDispatchAPI>(out);
        if (api) {
            outVar = api->getIDispatch();
        }

        return outVar;
    }
    
    template<> inline
    CComVariant makeComVariant<FB::JSAPIPtr>(const ActiveXBrowserHostPtr& host, const FB::variant& var)
    {
        CComVariant outVar;

        FB::JSAPIPtr obj(var.cast<FB::JSAPIPtr>());
        IDispatchAPIPtr api = ptr_cast<IDispatchAPI>(obj);
        if (api) {
            outVar = api->getIDispatch();
        } else {
            if (obj) {
                // Add obj to the list of shared_ptrs that browserhost keeps
                host->retainJSAPIPtr(obj);
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
        IDispatchAPIPtr api = ptr_cast<IDispatchAPI>(obj);
        if (api) {
            outVar = api->getIDispatch();
        } else {
            if (obj) {
                outVar = host->getJSAPIWrapper(obj);
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
        IDispatchAPIPtr api = ptr_cast<IDispatchAPI>(obj);
        if (api) {
            outVar = api->getIDispatch();
        } else {
            if (obj) {
                FB::JSAPIPtr ptr(var.convert_cast<FB::JSAPIPtr>());
                host->retainJSAPIPtr(ptr);
                outVar = getFactoryInstance()->createCOMJSObject(host, ptr);
                outVar.pdispVal->Release();
            } else {
                outVar.ChangeType(VT_NULL);
            }
        }

        return outVar;
    }
    
    namespace select_ccomvariant_builder
    {        
        template<class T>
        ComVariantBuilder isArithmetic(const boost::true_type& /* is_arithmetic */)
        {
            return &makeArithmeticComVariant<T>;
        }
        
        template<class T>
        ComVariantBuilder isArithmetic(const boost::false_type& /* is_arithmetic */)
        {
            return &makeComVariant<T>;
        }
        
        template<class T>
        ComVariantBuilder select()
        {
            return isArithmetic<T>(boost::is_arithmetic<T>());
        }
    }
} }

#endif

