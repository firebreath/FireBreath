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
#pragma once
#ifndef H_VARIANTLIST
#define H_VARIANTLIST

#include "APITypes.h"

namespace FB
{
    ///////////////////////////////////
    // detail forward declarations
    
    namespace detail 
    { 
        class VariantListInserter; 
    }

    ///////////////////////////////////
    // declarations

    /// @brief Create a FB::VariantList from any STL-style container (i.e. exposes begin() and end())
    /// @param cont A STL-style container.
    /// @return A FB::VariantList constructed from the cont.
    template<typename Cont>
    inline FB::VariantList make_variant_list(const Cont& cont);
    /// @brief Create a FB::VariantList from the range [begin, end).
    /// @param begin the start of the range
    /// @param end the end of the range
    /// @return A FB::VariantList containing variants constructed from the contents of the range.
    template<class InputIterator>
    inline FB::VariantList make_variant_list(InputIterator begin, InputIterator end);
    /// @brief Fill a FB::VariantList with the contents of the range [begin, end). 
    /// @param begin the start of the range
    /// @param end the end of the range
    /// @param result An iterator to the begin of a VariantList range big enough to hold last-first values.
    template<class InputIterator>
    inline void make_variant_list(InputIterator begin, InputIterator end, FB::VariantList::iterator result);

    /// @brief Convert a FB::VariantList to STL-style container (i.e. supports value_type and back-insert-iterators).
    /// @return A Cont with the converted contents of the FB::VariantList.
    template<class Cont>
    inline Cont convert_variant_list(const FB::VariantList& v);
    /// @brief Fill to with the contents of from, converted to Cont::value_type.
    /// @param from The input.
    /// @param to The container to store the converted values in.
    template<class Cont>
    inline void convert_variant_list(const FB::VariantList& from, Cont& to);
    /// @brief Convert a range of FB::VariantList to STL-style container (i.e. supports value_type and back-insert-iterators).
    /// @return A Cont with the converted contents of the FB::VariantList range.
    template<class Cont>
    inline Cont convert_variant_list(FB::VariantList::const_iterator begin,
                              FB::VariantList::const_iterator end);
    /// @brief Fills the range [result, result+(end-begin)) with the contents of the range [begin,end), converted to To
    /// @param begin the start of the range
    /// @param end the end of the range
    /// @param result Out parameter, shall be an output iterator.
    template<typename To, class OutputIterator>
    inline void convert_variant_list(FB::VariantList::const_iterator begin,
                              FB::VariantList::const_iterator end,
                              OutputIterator result);

    /// @brief Allows convenient creation of an FB::VariantList.
    /// @return A helper type that overloads operator() for insertion and is convertible to FB::VariantList.
    ///
    /// Examples: 
    /// @code
    /// FB::VariantList vars = FB::variant_list_of(1)("2")(3.4);
    /// FireEvent("randomDiceRoll", FB::variant_list_of(4));
    /// @endcode
    inline FB::detail::VariantListInserter variant_list_of(FB::variant v);
    /// @brief Convenience overload to create an empty FB::VariantList.
    /// @return An empty FB::VariantList.
    inline FB::VariantList variant_list_of();

    ///////////////////////////////////
    // detail definitions

    namespace detail
    {
        // helper for variant_list_of
        class VariantListInserter
        {
        public:
            VariantListInserter() : m_l() {}
            VariantListInserter(FB::variant v) : m_l(1, v) {}
            VariantListInserter& operator()(FB::variant v)
            {
                m_l.insert(m_l.end(), v);
                return *this;
            }
            operator FB::VariantList& ()
            {
                return m_l;
            }
            operator FB::variant()
            {
                return FB::variant(m_l);
            }
        private:
            FB::VariantList m_l;
        };
    }

    ///////////////////////////////////
    // definitions

    inline FB::detail::VariantListInserter 
    variant_list_of(FB::variant v)
    {
        return FB::detail::VariantListInserter(v);
    }

    inline FB::VariantList
    variant_list_of()
    {
        return FB::VariantList();
    }

    template<class InputIterator>
    inline void make_variant_list(InputIterator begin, 
                           InputIterator end, 
                           FB::VariantList::iterator result)
    {
        while(begin != end)
            *result++ = *begin++;
    }

    template<class InputIterator>
    inline FB::VariantList make_variant_list(InputIterator begin, InputIterator end)
    {
        FB::VariantList result(end-begin);
        std::copy(begin, end, result.begin());
        return result;
    }

    template<class Cont>
    inline FB::VariantList make_variant_list(const Cont& c)
    {
        FB::VariantList result;
        std::copy(c.begin(), c.end(), std::back_inserter(result));
        return result;
    }

    template<typename To, class OutputIterator>
    inline void convert_variant_list(FB::VariantList::const_iterator begin,
                              FB::VariantList::const_iterator end,
                              OutputIterator result)
    {
        while(begin != end){
            *result++ = (begin++)->convert_cast<To>();
        }
    }

    template<class Cont>
    inline Cont convert_variant_list(FB::VariantList::const_iterator begin,
                              FB::VariantList::const_iterator end)
    {
        Cont to;
        convert_variant_list<typename Cont::value_type>(begin, end, std::back_inserter(to));
        return to;
    }

    template<class Cont>
    inline Cont convert_variant_list(const FB::VariantList& from)
    {
        Cont to;
        convert_variant_list<typename Cont::value_type>(from.begin(), from.end(), std::back_inserter(to));
        return to;
    }

    template<class Cont>
    inline void convert_variant_list(const FB::VariantList& from, Cont& to)
    {
        convert_variant_list<typename Cont::value_type>(from.begin(), from.end(), std::back_inserter(to));
    }
}

#endif
