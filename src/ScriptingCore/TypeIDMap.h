/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 15, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_STRINGIDMAP
#define H_FB_STRINGIDMAP

#include <map>
#include <vector>
#include <string>

#include "APITypes.h"

namespace FB {

    template <class IDTYPE>
    class TypeIDMap
    {
    protected:
        class compare_variants { // simple comparison function
           public:
               bool operator()(const FB::variant &x, const FB::variant &y) const
               {
                   return x.lessthan(y);
               }
        };

        class compare_numerics { // simple numeric comparison function
            public:
                bool operator()(const IDTYPE &x, const IDTYPE &y) const
                {
                    long long x2 = (long long)(x);
                    long long y2 = (long long)(y);
                    return x2 < y2;
                }
        };

        typedef std::map<IDTYPE, FB::variant, compare_numerics> IdVariantMap;
        typedef std::map<FB::variant, IDTYPE, compare_variants> VariantIdMap;

    public:
        TypeIDMap(IDTYPE startValue) : nextId((unsigned long)startValue) { }
        virtual ~TypeIDMap() { }

        void setIdForValue(IDTYPE id, variant val)
        {
            if (m_idVariant.find(id) != m_idVariant.end()
                || m_variantId.find(val) != m_variantId.end()) {
                throw std::runtime_error("ID or value already defined");
            }
            m_variantId[val] = id;
            m_idVariant[id] = val;
        }

        IDTYPE getIdForValue(variant val)
        {
            typename VariantIdMap::iterator it = m_variantId.find(val);

            if (it != m_variantId.end()) {
                return it->second;
            } else {
                IDTYPE newId = (IDTYPE)nextId++;
                m_variantId[val] = newId;
                m_idVariant[newId] = val;
                return newId;
            }
        }

        std::type_info& idGetType(IDTYPE id)
        {
            typename IdVariantMap::iterator it = m_idVariant.find(id);

            if (it != m_idVariant.end()) {
                return it->second.get_type();
            } else {
                throw typeid(void);
            }
        }

        bool idExists(IDTYPE id)
        {
            return m_idVariant.find(id) != m_idVariant.end();
        }

        template <class T>
        bool idIsType(IDTYPE id)
        {
            typename IdVariantMap::iterator it = m_idVariant.find(id);

            if (it != m_idVariant.end()) {
                return it->second.get_type() == typeid(T);
            } else {
                return false;
            }
        }

        template <class T>
        T getValueForId(IDTYPE id)
        {
            typename IdVariantMap::iterator it = m_idVariant.find(id);

            if (it != m_idVariant.end()) {
                T retVal = (*it).second.template cast<T>();
                return retVal;
            } else {
                throw FB::bad_variant_cast(typeid(void), typeid(T));
            }
        }

    protected:
        IdVariantMap m_idVariant;
        VariantIdMap m_variantId;
        unsigned long nextId;

    };

};

#endif // H_FB_STRINGIDMAP
