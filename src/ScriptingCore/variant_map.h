/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    Dec 13, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_VARIANT_MAP
#define H_VARIANT_MAP

#include "APITypes.h"

namespace FB
{
    ///////////////////////////////////
    // detail forward declarations
    
    namespace detail 
    { 
        template<typename T>
        class VariantMapInserter; 
    }

    ///////////////////////////////////
    // declarations

    /// @brief Allows convenient creation of an FB::VariantMap.
    /// @return A helper type that overloads operator() for insertion and is convertible to FB::VariantMap.
    ///
    /// Examples: 
    /// @code
    /// typedef std::map<std::string, FB::variant> StringVariantMap;
    /// StringVariantMap varMap = FB::variant_map_of<std::string>("a",1)("b","2")("c",3.4);
    /// FireEvent("randomDiceRoll", FB::variant_map_of<std::string>("foo",42));
    /// std::string x = "xyz";
    /// StringVariantMap varMap2 = FB::variant_map_of(x, 1);
    /// @endcode
    template<typename T>
    FB::detail::VariantMapInserter<T> variant_map_of(const T& t, const FB::variant& v);
    
    /// @brief Convenience overload to create an empty variant map.
    /// @return An empty variant map.
    template<typename T>
    std::map<T,FB::variant> variant_map_of();

    ///////////////////////////////////
    // detail definitions

    namespace detail
    {
        // helper for variant_map_of
        template<typename T>
        class VariantMapInserter
        {
        public:
            VariantMapInserter() : m_m() {}
            VariantMapInserter(const T& t, const FB::variant& v) : m_m() 
            {
                (*this)(t,v);
            }
            VariantMapInserter<T>& operator()(const T& t, const FB::variant& v)
            {
                m_m.insert(m_m.end(), std::make_pair(t,v));
                return *this;
            }
            operator std::map<T,FB::variant> ()
            {
                return m_m;
            }
            operator FB::variant()
            {
                return FB::variant(m_m);
            }
        private:
            std::map<T,FB::variant> m_m;
        };
    }

    ///////////////////////////////////
    // definitions

    template<typename T>
    FB::detail::VariantMapInserter<T> 
    variant_map_of(const T& t, const FB::variant& v)
    {
        return FB::detail::VariantMapInserter<T>(t, v);
    }

    template<typename T>
    std::map<T,FB::variant>
    variant_map_of()
    {
        return std::map<T, FB::variant>();
    }
}

#endif

