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
#include <memory>

#define FB_FORWARD_PTR(x) class x; \
    using x ## Ptr = std::shared_ptr<x>; \
    using x ## WeakPtr = std::weak_ptr<x>; \
    using x ## ConstPtr = std::shared_ptr<const x>; \
    using x ## WeakConstPtr = std::weak_ptr<const x>;

// get rid of "unused variable" warnings
#define FB_UNUSED_VARIABLE(x) ((void)(x))

#endif // FBPointers_h__

