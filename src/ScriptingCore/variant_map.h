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

    // convenience creation of variant map similar to boost::assign but passable as a temporary
    template<typename T>
    FB::detail::VariantMapInserter<T> variant_map_of(const T& t, const FB::variant& v);

    template<typename T>
    FB::detail::VariantMapInserter<T> variant_map_of();

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
    FB::detail::VariantMapInserter<T>
    variant_map_of()
    {
        return FB::detail::VariantMapInserter<T>();
    }
}

#endif
