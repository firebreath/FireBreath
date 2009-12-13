/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 24, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_VARIANTLIST
#define H_VARIANTLIST

#include "APITypes.h"

namespace FB
{
	namespace detail
	{
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

	FB::detail::VariantListInserter variant_list_of(FB::variant v)
	{
		return FB::detail::VariantListInserter(v);
	}

	FB::detail::VariantListInserter variant_list_of()
	{
		return FB::detail::VariantListInserter();
	}
}

#endif