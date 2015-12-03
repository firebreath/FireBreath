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
#include "AlienWyrmling.h"
#include "variant_list.h"
#include "fbjson.h"

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

    using VariantPreprocessorBuilder = FB::variant (*)(FB::variant, WyrmBrowserHostPtr);
    using VariantPreprocessorMap = std::map<std::type_info const*, VariantPreprocessorBuilder, type_info_less>;

    const VariantPreprocessorMap& getJsonVariantPreprocessorMap();

    FB::variant preprocessVariant(FB::variant var, WyrmBrowserHostPtr host);

    template<class T>
    FB::variant makeValue(FB::variant var, WyrmBrowserHostPtr host)
    {
        // Most types don't need to be translated
        return var;
    }
    
    template<> inline
    FB::variant makeValue<const std::exception>(FB::variant var, WyrmBrowserHostPtr host) {
        auto e = var.cast<const std::exception>();
        return FB::VariantMap{ {"$type", "error"}, {"message", e.what()} };
    }
    
    template<> inline
    FB::variant makeValue<const std::exception_ptr>(FB::variant var, WyrmBrowserHostPtr host) {
        auto ep = var.cast<const std::exception_ptr>();
        if(ep) {
            try {
                std::rethrow_exception(ep);
            } catch(const std::exception& e) {
                return FB::VariantMap{ {"$type", "error"}, {"message", e.what()} };
            } catch(...) {}
        }
        return FB::VariantMap{ {"$type", "error"}, {"message", "Unknown exception"} };
    }
    
    template<> inline
    FB::variant makeValue<FB::JSAPIPtr>(FB::variant var, WyrmBrowserHostPtr host)
    {
        auto ptr = var.cast<FB::JSAPIPtr>();
        auto ling = host->getWyrmling(ptr);

        return FB::VariantMap{ {"$type", "ref"}, {"data", FB::VariantList{host->getSpawnId(), ling.getObjectId()}} };
    }
    template<> inline
    FB::variant makeValue<FB::JSAPIWeakPtr>(FB::variant var, WyrmBrowserHostPtr host)
    {
        auto ptr = var.cast<FB::JSAPIWeakPtr>();
        auto ling = host->getWyrmling(ptr);

        return FB::VariantMap{ {"$type", "ref"}, {"data", FB::VariantList{host->getSpawnId(), ling.getObjectId()}} };
    }
    
    template<> inline
    FB::variant makeValue<WyrmlingKey>(FB::variant var, WyrmBrowserHostPtr host)
    {
        auto ling = var.cast<WyrmlingKey>();
        
        return FB::VariantMap{ {"$type", "ref"}, {"data", FB::VariantList{ling.first, ling.second}} };
    }

    template<> inline
    FB::variant makeValue<FB::VariantList>(FB::variant var, WyrmBrowserHostPtr host)
    {
        FB::VariantList out;
        for (auto &cur : var.cast<FB::VariantList>()) {
            out.emplace_back(FB::FireWyrm::preprocessVariant(cur, host));
        }
        return out;
    }
    
    template<> inline
    FB::variant makeValue<FB::VariantMap>(FB::variant var, WyrmBrowserHostPtr host)
    {
        FB::VariantMap out;
        for (auto &cur : var.cast<FB::VariantMap>()) {
            out[cur.first] = FB::FireWyrm::preprocessVariant(cur.second, host);
        }
        return out;
    }
    
    template<> inline
    FB::variant makeValue<FB::JSObjectPtr>(FB::variant var, WyrmBrowserHostPtr host)
    {
        // TODO: This could result in some really inefficient access if something is passed back into
        // our plugin; for now we'll just let it slide, but eventually we should find a way to get
        // back to the original JSAPI object
        auto ptr = std::dynamic_pointer_cast<FB::FireWyrm::AlienWyrmling>(var.cast<FB::JSObjectPtr>());
        if (ptr) {
            auto key = ptr->getWyrmlingKey();
            return FB::VariantMap{ {"$type", "local-ref"}, {"data", FB::VariantList{key.first, key.second}} };
        } else {
            return makeValue<FB::JSAPIPtr>(std::dynamic_pointer_cast<FB::JSAPI>(var.cast<FB::JSObjectPtr>()), host);
        }
    }

    namespace preprocess_variant
    {
        template<class T>
        VariantPreprocessorBuilder isIntegral(const boost::true_type& /* is_integral */)
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
        VariantPreprocessorBuilder isIntegral(const boost::false_type& /* is_integral */)
        {
            BOOST_STATIC_ASSERT((boost::is_floating_point<T>::value));
            return &makeValue<double>;
        }

        template<class T>
        VariantPreprocessorBuilder isArithmetic(const boost::true_type& /* is_arithmetic */)
        {
            return isIntegral<T>(boost::is_integral<T>());
        }

        template<class T>
        VariantPreprocessorBuilder isArithmetic(const boost::false_type& /* is_arithmetic */)
        {
            return &makeValue<T>;
        }

        template<class T>
        VariantPreprocessorBuilder select()
        {
            return isArithmetic<T>(boost::is_arithmetic<T>());
        }
    }
} }

#endif // FB_H_FWVARIANTUTIL
