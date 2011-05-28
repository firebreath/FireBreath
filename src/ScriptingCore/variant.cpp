/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Dec 30, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Richard Bateman, Firebreath development team
\**********************************************************/

#include "variant.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

using FB::variant;

variant FB::variant_detail::conversion::make_variant(const char* x) {
    return variant(std::string(x), true);
}
variant FB::variant_detail::conversion::make_variant(const wchar_t* x) {
    return variant(std::wstring(x), true);
}
///////////////////////////////////////////////////
// variant convert_cast helpers
//
// These functions are called to process any
// values assigned to the variant.  For example,
// all const char* parameters are converted to
// std::string
///////////////////////////////////////////////////

const void FB::variant_detail::conversion::convert_variant(const FB::variant&, const FB::variant_detail::conversion::type_spec<void>) {
    return;
}

const FB::variant& FB::variant_detail::conversion::convert_variant(const FB::variant& var, const FB::variant_detail::conversion::type_spec<FB::variant>) {
    return var;
}

const FB::FBNull FB::variant_detail::conversion::convert_variant( const FB::variant&, const type_spec<FB::FBNull> )
{
    return FB::FBNull();
}

const FB::FBVoid FB::variant_detail::conversion::convert_variant( const FB::variant&, const type_spec<FB::FBVoid> )
{
    return FB::FBVoid();
}

FB::variant FB::variant_detail::conversion::make_variant(const boost::tribool& val) {
    if (val == boost::indeterminate)
        return FB::variant();
    else
        return (bool)val;
}
boost::tribool FB::variant_detail::conversion::convert_variant( const FB::variant& var, const type_spec<boost::tribool>& )
{
    if (var.is_null() || var.empty())
        return boost::indeterminate;
    else
        return var.convert_cast<bool>();
}

