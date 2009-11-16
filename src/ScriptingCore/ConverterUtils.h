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

namespace FB { 
	namespace detail 
	{
		template<typename T>
		struct plain_type {
			typedef typename boost::remove_const<typename boost::remove_reference<T>::type>::type type;
		};

		template<typename To, typename From>
		struct converter {
			static inline To convert(const From& f);
			static inline To convert(const From& f, size_t index);
		};

		namespace methods
		{
			template<typename LastArgType>
			inline
			bool checkArgumentCount(const FB::VariantList& l, size_t n);

			template<typename ArgType>
			inline
			ArgType convertLastArgument(const FB::VariantList& l, size_t n);
		}
	} // namespace detail

	template<typename To, typename From>
	inline
	To convertArgument(const From& from)
	{
		return FB::detail::converter<To, From>::convert(from);
	}

	template<typename To, typename From>
	inline
	To convertArgument(const From& from, const size_t index)
	{
		return FB::detail::converter<To, From>::convert(from, index);
	}
} // namespace FB

namespace FB { namespace detail
{
	template<typename To>
	struct converter<To, FB::variant>
	{
		static inline To convert(const FB::variant& from)
		{
			return from.convert_cast<To>();
		}

		static inline To convert(const FB::variant& from, size_t index)
		{
			try {
				return from.convert_cast<To>();
			} catch(FB::bad_variant_cast& e) {
				std::stringstream ss;
				ss << "Invalid argument conversion "
				   << "from " << e.from 
				   << " to "  << e.to 
				   << " at index " << index;
				throw FB::invalid_arguments(ss.str());
			}
		}
	};

	template<>
	struct converter<FB::variant, FB::variant>
	{
		static inline FB::variant convert(const FB::variant& from)
		{
			return from;
		}

		static inline FB::variant convert(const FB::variant& from, size_t index)
		{
			return from;
		}
	};

	namespace methods
	{
		template<typename LastArgType>
		inline
		bool checkArgumentCount(const FB::VariantList& l, size_t n)
		{
			return (l.size() == n);
		}

		template<>
		inline
		bool checkArgumentCount<FB::VariantList>(const FB::VariantList& l, size_t n)
		{
			return (l.size() >= (n-1));
		}

		template<typename ArgType>
		inline
		ArgType convertLastArgument(const FB::VariantList& l, size_t n)
		{
			return FB::convertArgument<ArgType, FB::variant>(l.back(), n);
		}

		template<>
		inline
		FB::VariantList convertLastArgument<FB::VariantList>(const FB::VariantList& l, size_t n)
		{
			return FB::VariantList(l.begin()+(n-1), l.end());
		}
	}
} } // namespace FB { namespace detail

#endif // #if !defined(CONVERTER_UTILS_H)