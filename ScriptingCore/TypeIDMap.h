/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 15, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

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
               bool operator()(const cdiggins::any &x, const cdiggins::any &y)
               {
                   return x.lessthan(y);
               }
        };

        class compare_numerics { // simple numeric comparison function
            public:
                bool operator()(const IDTYPE &x, const IDTYPE &y)
                {
                    long long x2 = (long long)(x);
                    long long y2 = (long long)(y);
                    return x2 < y2;
                }
        };

        typedef std::map<IDTYPE, cdiggins::any, compare_numerics> IdVariantMap;
        typedef std::map<cdiggins::any, IDTYPE, compare_variants> VariantIdMap;

    public:
        TypeIDMap(IDTYPE startValue) : nextId((unsigned long)startValue) { }
        virtual ~TypeIDMap() { }

        IDTYPE getIdForValue(variant val)
        {
            VariantIdMap::iterator it = m_variantId.find(val);

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
            IdVariantMap::iterator it = m_idVariant.find(id);

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
            IdVariantMap::iterator it = m_idVariant.find(id);

            if (it != m_idVariant.end()) {
                return it->second.get_type() == typeid(T);
            } else {
                return false;
            }
        }

        template <class T>
        T getValueForId(IDTYPE id)
        {
            IdVariantMap::iterator it = m_idVariant.find(id);

            if (it != m_idVariant.end()) {
                return it->second.cast<T>();
            } else {
                throw cdiggins::bad_any_cast(typeid(void), typeid(T));
            }
        }

    protected:
        IdVariantMap m_idVariant;
        VariantIdMap m_variantId;
        unsigned long nextId;

    };

};

#endif // H_FB_STRINGIDMAP
