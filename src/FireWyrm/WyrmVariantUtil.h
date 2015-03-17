/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Mar 11, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2015 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef FB_H_FWVARIANTUTIL
#define FB_H_FWVARIANTUTIL

#include <map>
#include <string>
#include <typeinfo>

#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/integer_traits.hpp>

#include "APITypes.h"
#include "LocalWyrmling.h"
#include "variant_list.h"
#include "fbjson.h"

using Json::Value;

namespace FB { namespace FireWyrm
{
    FB_FORWARD_PTR(WyrmBrowserHost);

    struct type_info_less
    {
        bool operator() (const std::type_info* const lhs, const std::type_info* const rhs) const
        {
            return lhs->before(*rhs) ? true : false;
        }
    };

    using ValueBuilder = Value (*)(FB::variant, WyrmBrowserHostPtr);
    using ValueBuilderMap = std::map<std::type_info const*, ValueBuilder, type_info_less>;

    const ValueBuilderMap& getJsonValueBuilderMap();

    Value getValueForVariant(FB::variant var, WyrmBrowserHostPtr host) {
        auto builderMap = getJsonValueBuilderMap();
        const std::type_info& type = var.get_type();
        auto it = builderMap.find(&type);

        if (it == builderMap.end()) {
            // unhandled type :(
            return Json::nullValue;
        }

        return (it->second)(var, host);
    }

    template<class T>
    Value makeValue(FB::variant var, WyrmBrowserHostPtr host)
    {
        // If it's not a known type this will throw an exception
        return Value(var.convert_cast<T>());
    }

    template<> inline
    Value makeValue<std::wstring>(FB::variant var, WyrmBrowserHostPtr host)
    {
        return Value(FB::wstring_to_utf8(var.convert_cast<std::wstring>()));
    }

    template<> inline
    Value makeValue<FB::FBVoid>(FB::variant var, WyrmBrowserHostPtr host)
    {
        Value obj(Json::objectValue);
        obj["$type"] = "undefined";
        return obj;
    }

    template<> inline
    Value makeValue<FB::FBNull>(FB::variant var, WyrmBrowserHostPtr host)
    {
        return Json::nullValue;
    }

    template<> inline
    Value makeValue<const std::exception>(FB::variant var, WyrmBrowserHostPtr host) {
        Value out(Json::objectValue);
        const std::exception e = var.cast<const std::exception>();
        out["$type"] = "error";
        out["message"] = e.what();
        return out;
    }

    template<> inline
        Value makeValue<FB::VariantList>(FB::variant var, WyrmBrowserHostPtr host) {
        Value out(Json::arrayValue);

        for (auto cur : var.cast<FB::VariantList>()) {
            out.append(getValueForVariant(var, host));
        }
        return out;
    }

    template<> inline
    Value makeValue<FB::VariantMap>(FB::variant var, WyrmBrowserHostPtr host)
    {
        Value out(Json::objectValue);

        for (auto cur : var.cast<FB::VariantMap>()) {
            out[cur.first] = getValueForVariant(cur.second, host);
        }
        return out;
    }

    template<> inline
    Value makeValue<FB::JSAPIPtr>(FB::variant var, WyrmBrowserHostPtr host)
    {
        auto ptr = var.cast<FB::JSAPIPtr>();
        auto ling = host->getWyrmling(ptr);

        Value out(Json::objectValue);
        out["$type"] = "ref";
        out["data"] = Json::arrayValue;
        out["data"].append(host->getSpawnId());
        out["data"].append(ling.getObjectId());
        out["data"] = ling.getObjectId();
        return out;
    }
    template<> inline
    Value makeValue<FB::JSAPIWeakPtr>(FB::variant var, WyrmBrowserHostPtr host)
    {
        auto ptr = var.cast<FB::JSAPIWeakPtr>();
        auto ling = host->getWyrmling(ptr);

        Value out(Json::objectValue);
        out["$type"] = "ref";
        out["data"] = ling.getObjectId();
        return out;
    }

    template<> inline
    Value makeValue<FB::JSObjectPtr>(FB::variant var, WyrmBrowserHostPtr host)
    {
        // TODO: This could result in some really inefficient access if something is passed back into
        // our plugin; for now we'll just let it slide, but eventually we should find a way to get
        // back to the original JSAPI object
        return makeValue<FB::JSAPIPtr>(std::dynamic_pointer_cast<FB::JSAPI>(var.cast<FB::JSObjectPtr>()), host);
    }

    namespace select_jsonvalue_builder
    {
        template<class T>
        ValueBuilder isIntegral(const boost::true_type& /* is_integral */)
        {
            if (boost::is_same<T, bool>::value) {
                return &makeValue<bool>;
            }

            if ((boost::is_signed<T>::value && (sizeof(T) <= sizeof(int32_t))) || (boost::is_unsigned<T>::value && (sizeof(T) <= sizeof(int32_t)/2))) {
                // max value of T fits into int32_t
                return &makeValue<int32_t>;
            }

            // max value of T doesn't fit into int32_t, use string instead
            return &makeValue<std::string>;
        }

        template<class T>
        ValueBuilder isIntegral(const boost::false_type& /* is_integral */)
        {
            BOOST_STATIC_ASSERT((boost::is_floating_point<T>::value));
            return &makeValue<double>;
        }

        template<class T>
        ValueBuilder isArithmetic(const boost::true_type& /* is_arithmetic */)
        {
            return isIntegral<T>(boost::is_integral<T>());
        }

        template<class T>
        ValueBuilder isArithmetic(const boost::false_type& /* is_arithmetic */)
        {
            return &makeValue<T>;
        }

        template<class T>
        ValueBuilder select()
        {
            return isArithmetic<T>(boost::is_arithmetic<T>());
        }
    }
} }

#endif // FB_H_FWVARIANTUTIL
