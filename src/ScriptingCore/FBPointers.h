#pragma once
#ifndef FBPointers_h__
#define FBPointers_h__

/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Sept 24, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/make_shared.hpp>

#define FB_FORWARD_PTR(x) class x; \
    typedef boost::shared_ptr<x> x ## Ptr; \
    typedef boost::weak_ptr<x> x ## WeakPtr;

// get rid of "unused variable" warnings
#define FB_UNUSED_VARIABLE(x) ((void)(x))

#endif // FBPointers_h__

