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

#pragma once
#ifndef H_FB_STRINGIDMAP
#define H_FB_STRINGIDMAP

#include <map>
#include <vector>
#include <string>

#include "APITypes.h"

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  TypeIDMap
    ///
    /// @brief  Bidirectional map between an identifier and a variant
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    template <class IDTYPE>
    class TypeIDMap
    {
    protected:
        class compare_variants { // simple comparison function
           public:
               bool operator()(const FB::variant &x, const FB::variant &y) const
               {
                   return x < y;
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

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @typedef    std::map<IDTYPE, FB::variant, compare_numerics> IdVariantMap
        ///
        /// @brief  Defines an alias representing the map from the id type to a variant.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        typedef std::map<IDTYPE, FB::variant, compare_numerics> IdVariantMap;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @typedef    std::map<FB::variant, IDTYPE, compare_variants> VariantIdMap
        ///
        /// @brief  Defines an alias representing the map from a variant to the id
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        typedef std::map<FB::variant, IDTYPE, compare_variants> VariantIdMap;

    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn TypeIDMap::TypeIDMap(IDTYPE startValue)
        ///
        /// @brief  Constructor. 
        ///
        /// @param  startValue  The value for the first ID to auto-assign
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        TypeIDMap(IDTYPE startValue) : nextId((unsigned long)startValue) { }
        virtual ~TypeIDMap() { }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void TypeIDMap::setIdForValue(IDTYPE idt, const variant& val)
        ///
        /// @brief  Sets an identifier for value. 
        ///
        /// @exception  std::runtime_error  Thrown if the identifier is already mapped to something 
        ///
        /// @param  idt The identifier 
        /// @param  val The value
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void setIdForValue(IDTYPE idt, const variant& val)
        {
            if (m_idVariant.find(idt) != m_idVariant.end()
                || m_variantId.find(val) != m_variantId.end()) {
                throw std::runtime_error("ID or value already defined");
            }
            m_variantId[val] = idt;
            m_idVariant[idt] = val;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn IDTYPE TypeIDMap::getIdForValue(const variant& val)
        ///
        /// @brief  Gets the identifier assigned to the given value; if none is yet assigned, assigns a
        ///         sequential identifier to the provided value
        ///
        /// @param  val The value. 
        ///
        /// @return The identifier assigned to value. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        IDTYPE getIdForValue(const variant& val)
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

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn const std::type_info& TypeIDMap::idGetType(IDTYPE idt)
        ///
        /// @brief  Returns the type of the value the given identifier is assigned to.
        ///
        /// @param  idt The identifier
        ///
        /// @return the type for comparison with typeid
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        const std::type_info& idGetType(IDTYPE idt)
        {
            typename IdVariantMap::iterator it = m_idVariant.find(idt);

            if (it != m_idVariant.end()) {
                return it->second.get_type();
            } else {
                return typeid(void);
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn bool TypeIDMap::idExists(IDTYPE idt)
        ///
        /// @brief  Queries if a given identifier has been assigned to anything. 
        ///
        /// @param  idt The identifier to check. 
        ///
        /// @return true if yes, false if no. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        bool idExists(IDTYPE idt)
        {
            return m_idVariant.find(idt) != m_idVariant.end();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template <class T> bool TypeIDMap::idIsType(IDTYPE idt)
        ///
        /// @brief  Queries if the provided identifier is assigned to a value of the given type 
        ///
        /// Example:
        /// @code
        ///      bool isStr = idMap.idIsType<std::string>(47);
        /// @endcode
        /// 
        /// @param  idt The identifier to check. 
        ///
        /// @return true if it is the given type, false if it is not. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template <class T>
        bool idIsType(IDTYPE idt)
        {
            typename IdVariantMap::iterator it = m_idVariant.find(idt);

            if (it != m_idVariant.end()) {
                return it->second.get_type() == typeid(T);
            } else {
                return false;
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template <class T> T TypeIDMap::getValueForId(IDTYPE idt)
        ///
        /// @brief  Gets the value assigned to the identifier. 
        ///
        /// @exception  FB::bad_variant_cast    Thrown if the provided type is not valid for the value or if
        ///                                     the identifier is not assigned to anything.
        ///
        /// @param  idt The identifier to check. 
        ///
        /// @return The value of type T that the identifier is assigned to
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template <class T>
        T getValueForId(IDTYPE idt)
        {
            typename IdVariantMap::iterator it = m_idVariant.find(idt);

            if (it != m_idVariant.end()) {
                T retVal = (*it).second.template convert_cast<T>();
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

