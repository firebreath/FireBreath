
#if !defined(CONVERTER_UTILS_H)
#define CONVERTER_UTILS_H

#if defined(_MSC_VER)
#  pragma once
#endif

#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_const.hpp>

namespace FB { namespace detail {
	namespace b = boost;

	template<typename T>
	struct plain_type {
		typedef typename boost::remove_const<typename b::remove_reference<T>::type>::type type;
	};
} } // FB::detail

#endif // #if !defined(CONVERTER_UTILS_H)