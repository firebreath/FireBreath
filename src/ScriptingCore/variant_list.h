/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    Dec 13, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
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
    FB::VariantList make_variant_list(const Cont&);

    // convert variant list to STL-style container (i.e. supports value_type and back-insert-iterators)
    template<class Cont>
    Cont convert_variant_list(const FB::VariantList& v);

    // convenience creation of variant list similar to boost::assign but passable as a temporary
    FB::detail::VariantListInserter variant_list_of(FB::variant v);
    FB::detail::VariantListInserter variant_list_of();

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

    FB::detail::VariantListInserter 
    variant_list_of(FB::variant v)
    {
        return FB::detail::VariantListInserter(v);
    }

    FB::detail::VariantListInserter 
    variant_list_of()
    {
        return FB::detail::VariantListInserter();
    }

    template<class Cont>
    FB::VariantList make_variant_list(const Cont& c)
    {
        FB::VariantList vl;
        std::back_insert_iterator<FB::VariantList> inserter(vl);

        for(Cont::const_iterator it = c.begin() ; it<c.end(); ++it) {
            inserter = *it;
        }

        return vl;
    }

    template<class Cont>
    Cont convert_variant_list(const FB::VariantList& v)
    {
        typedef typename Cont::value_type T;
        Cont c;
        std::back_insert_iterator<Cont> inserter(c);

        for(FB::VariantList::const_iterator it = v.begin() ; it<v.end(); ++it) {
            inserter = it->convert_cast<T>();
        }

        return c;
    }
}

#endif
