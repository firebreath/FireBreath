/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    November 9, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#if !defined(CONVERTER_UTILS_H)
#define CONVERTER_UTILS_H

#if defined(_MSC_VER)
#  pragma once
#endif

#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_const.hpp>

namespace FB { namespace detail {
	template<typename T>
	struct plain_type {
		typedef typename boost::remove_const<typename boost::remove_reference<T>::type>::type type;
	};
} } // FB::detail

#endif // #if !defined(CONVERTER_UTILS_H)