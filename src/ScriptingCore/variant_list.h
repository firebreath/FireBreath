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

    // create variant list from any STL-style container (i.e. exposes begin() and end())
    template<typename Cont>
    inline FB::VariantList make_variant_list(const Cont&);
    template<class InputIterator>
    inline FB::VariantList make_variant_list(InputIterator begin, InputIterator end);
    template<class InputIterator>
    inline void make_variant_list(InputIterator begin, InputIterator end, FB::VariantList::iterator result);

    // convert variant list to STL-style container (i.e. supports value_type and back-insert-iterators)
    template<class Cont>
    inline Cont convert_variant_list(const FB::VariantList& v);
    template<class Cont>
    inline void convert_variant_list(const FB::VariantList& from, Cont& to);
    template<class Cont>
    inline Cont convert_variant_list(FB::VariantList::const_iterator begin,
                              FB::VariantList::const_iterator end);
    template<typename To, class Cont>
    inline void convert_variant_list(FB::VariantList::const_iterator begin,
                              FB::VariantList::const_iterator end,
                              typename Cont::iterator result);

    // convenience creation of variant list similar to boost::assign but passable as a temporary
    inline FB::detail::VariantListInserter variant_list_of(FB::variant v);
    inline FB::detail::VariantListInserter variant_list_of();

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

    inline FB::detail::VariantListInserter 
    variant_list_of()
    {
        return FB::detail::VariantListInserter();
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
