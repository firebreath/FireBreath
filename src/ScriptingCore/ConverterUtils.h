/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    November 9, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#if !defined(CONVERTER_UTILS_H)
#define CONVERTER_UTILS_H

#if defined(_MSC_VER)
#  pragma once
#endif

#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/vector.hpp>

namespace FB { 
    class variant;
    typedef std::vector<variant> VariantList;
    namespace detail 
    {
        // helper meta function that removes all pointers etc. from a type
        template<typename T>
        struct plain_type {
            typedef typename boost::remove_const<typename boost::remove_reference<T>::type>::type type;
        };

        // actual conversion functions are wrapped in this struct to allow partial specialization
        template<typename To, typename From>
        struct converter {
            static inline To convert(const From& f);
            static inline To convert(const From& f, size_t index);
        };

        // helper functions for method calls
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

    // "soft" conversion wrapper function
    template<typename To>
    inline
    To convertArgumentSoft(const FB::VariantList& args, const size_t index,
        typename boost::disable_if<boost::mpl::or_<
            FB::meta::is_optional<To>,
            boost::mpl::or_<
                boost::is_same<To, FB::variant>,
                boost::is_same<To, boost::tribool>
            >
        > >::type* p=0)
    {
        if (args.size() >= index)
            return FB::detail::converter<To, FB::variant>::convert(args[index-1], index);
        else {
            std::stringstream ss;
            ss << "Error: Argument " << index
               << "is not optional.";
            throw FB::invalid_arguments(ss.str());
        }
    }
    
    template<typename To>
    inline
    To convertArgumentSoft(const FB::VariantList& args, const size_t index,
        typename boost::enable_if<FB::meta::is_optional<To> >::type* p=0)
    {
        if (args.size() >= index)
            return FB::detail::converter<To, FB::variant>::convert(args[index-1], index);
        else
            return To(); // Empty optional argument
    }

    template<typename To>
    inline
    To convertArgumentSoft(const FB::VariantList& args, const size_t index,
        typename boost::enable_if<boost::is_same<To, boost::tribool> >::type* p=0)
    {
        if (args.size() >= index)
            return FB::detail::converter<To, FB::variant>::convert(args[index-1], index);
        else
            return boost::tribool(); // Empty variant argument
    }

    template<typename To>
    inline
    To convertArgumentSoft(const FB::VariantList& args, const size_t index,
        typename boost::enable_if<boost::is_same<To, FB::variant> >::type* p=0)
    {
        if (args.size() >= index)
            return FB::detail::converter<To, FB::variant>::convert(args[index-1], index);
        else
            return FB::variant(); // Empty variant argument
    }
    // conversion wrapper function
    template<typename To, typename From>
    inline
    To convertArgument(const From& from)
    {
        return FB::detail::converter<To, From>::convert(from);
    }

    // conversion wrapper function
    template<typename To, typename From>
    inline
    To convertArgument(const From& from, const size_t index, boost::disable_if< FB::meta::is_optional<To> >*p = 0)
    {
        return FB::detail::converter<To, From>::convert(from, index);
    }
} // namespace FB

namespace FB { namespace detail
{
    // specialization of converter for conversions from FB::variant
    template<typename To>
    struct converter<To, FB::variant>
    {
        static inline To convert(const FB::variant& from)
        {
            try {
                return from.convert_cast<To>();
            } catch(const FB::bad_variant_cast& e) {
                std::stringstream ss;
                ss << "Invalid argument conversion "
                   << "from " << e.from 
                   << " to "  << e.to;
                throw FB::invalid_arguments(ss.str());
            }
        }

        static inline To convert(const FB::variant& from, size_t index)
        {
            try {
                return from.convert_cast<To>();
            } catch(const FB::bad_variant_cast& e) {
                std::stringstream ss;
                ss << "Invalid argument conversion "
                   << "from " << e.from 
                   << " to "  << e.to 
                   << " at index " << index;
                throw FB::invalid_arguments(ss.str());
            }
        }
    };

    // specialization for conversion from variant to variant
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
        // checking argument count if last arg isn't a catch-all type
        template<typename LastArgType>
        inline
        bool checkArgumentCount(const FB::VariantList& l, size_t n)
        {
            if(l.size() != n) {
                std::stringstream ss;
                ss << "Invalid Argument Count, expected " << n << " arguments";
                throw FB::invalid_arguments(ss.str());
            }

            return true;
        }

        // if the last argument is a catch-all type, we only have a lower bound
        template<>
        inline
        bool checkArgumentCount<FB::CatchAll>(const FB::VariantList& l, size_t n)
        {
            if(l.size() < (n-1)) {
                std::stringstream ss;
                ss << "Invalid Argument Count, expected at least " << (n-1) << " arguments";
                throw FB::invalid_arguments(ss.str());
            }

            return true;
        }

        // default handling for the last parameter is just converting
        template<typename ArgType>
        inline
        ArgType convertLastArgument(const FB::VariantList& l, size_t n)
        {
            // If this is the last parameter and 
            if (l.size() > n) {
                std::stringstream ss;
                ss << "Too many arguments, expected " << n << ".";
                throw FB::invalid_arguments(ss.str());
            }
            return FB::convertArgumentSoft<ArgType>(l, n);
        }

        // if the last argument is CatchAll, fill it with all remaining parameters
        template<>
        inline
        FB::CatchAll convertLastArgument<FB::CatchAll>(const FB::VariantList& l, size_t n)
        {
            FB::CatchAll result;
            long count = (long)l.size() - ((long)n - 1);
            if(count < 1) 
                return result;            
            for( ; n <= l.size(); ++n)
                result.value.push_back(l[n-1]);
            return result;
        }
    } // namespace methods
} } // namespace FB { namespace detail

#endif // #if !defined(CONVERTER_UTILS_H)

