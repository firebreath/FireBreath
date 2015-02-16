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
#include <iterator>

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

    /// @brief Create a FB::VariantMap from any STL-style dictionary (i.e. exposes begin() and end())
    /// @param cont A STL-style container.
    /// @return A FB::VariantMap constructed from the cont.
    template<typename Dict>
    inline FB::VariantMap make_variant_map(const Dict& dict);
    /// @brief Create a FB::VariantMap from the range [begin, end).
    /// @param begin the start of the range
    /// @param end the end of the range
    /// @return A FB::VariantMap containing variants constructed from the contents of the range.
    template<class InputIterator>
    inline FB::VariantMap make_variant_map(InputIterator begin, InputIterator end);
    /// @brief Fill a FB::VariantMap with the contents of the range [begin, end). 
    /// @param begin the start of the range
    /// @param end the end of the range
    /// @param result An iterator to the begin of a VariantMap range big enough to hold last-first values.
    template<class InputIterator>
    inline void make_variant_map(InputIterator begin, InputIterator end, FB::VariantMap::iterator result);

    /// @brief Convert a FB::VariantMap to STL-style dictionary (i.e. supports value_type and insert-iterators).
    /// @return A Cont with the converted contents of the FB::VariantList.
    template<class Dict>
    inline Dict convert_variant_map(const FB::VariantMap& v);
    /// @brief Fill to with the contents of from, converted to Cont::value_type.
    /// @param from The input.
    /// @param to The container to store the converted values in.
    template<class Dict>
    inline void convert_variant_map(const FB::VariantMap& from, Dict& to);
    /// @brief Convert a range of FB::VariantMap to STL-style dictionary (i.e. supports value_type and insert-iterators).
    /// @return A Cont with the converted contents of the FB::VariantMap range.
    template<class Dict>
    inline Dict convert_variant_map(FB::VariantMap::const_iterator begin,
                              FB::VariantMap::const_iterator end);
    /// @brief Fills the range [result, result+(end-begin)) with the contents of the range [begin,end), converted to To
    /// @param begin the start of the range
    /// @param end the end of the range
    /// @param result Out parameter, shall be an output iterator.
    template<typename To, class OutputIterator>
    inline void convert_variant_map(FB::VariantMap::const_iterator begin,
                              FB::VariantMap::const_iterator end,
                              OutputIterator result);


    ///////////////////////////////////
    // definitions

    template<class InputIterator>
    inline void make_variant_map(InputIterator begin, 
                           InputIterator end, 
                           FB::VariantMap::iterator result)
    {
        while(begin != end)
            *result++ = *begin++;
    }

    template<class InputIterator>
    inline FB::VariantMap make_variant_map(InputIterator begin, InputIterator end)
    {
        FB::VariantMap result(end-begin);
        std::copy(begin, end, result.begin());
        return result;
    }

    template<class Dict>
    inline FB::VariantMap make_variant_map(const Dict& d)
    {
        FB::VariantMap result;
        std::copy(d.begin(), d.end(), std::inserter(result, result.begin()));
        return result;
    }

    template<typename To, class OutputIterator>
    inline void convert_variant_map(FB::VariantMap::const_iterator begin,
                              FB::VariantMap::const_iterator end,
                              OutputIterator result)
    {
        while(begin != end){
            FB::VariantMap::const_iterator it = begin++;
            *result++ = To(it->first, it->second.convert_cast<typename To::second_type>());
        }
    }

    template<class Dict>
    inline Dict convert_variant_map(FB::VariantMap::const_iterator begin,
                              FB::VariantMap::const_iterator end)
    {
        Dict to;
        convert_variant_map<typename Dict::value_type>(begin, end, std::inserter(to, to.begin()));
        return to;
    }

    template<class Dict>
    inline Dict convert_variant_map(const FB::VariantMap& from)
    {
        Dict to;
        convert_variant_map<typename Dict::value_type>(from.begin(), from.end(), std::inserter(to, to.begin()));
        return to;
    }

    template<class Dict>
    inline void convert_variant_map(const FB::VariantMap& from, Dict& to)
    {
        convert_variant_map<typename Dict::value_type>(from.begin(), from.end(), std::inserter(to, to.begin()));
    }
}

#endif

