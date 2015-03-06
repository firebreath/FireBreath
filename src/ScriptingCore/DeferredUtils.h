/**********************************************************\ 
 Original Author: Richard Bateman (taxilian)

 Created:    Mar 4, 2015
 License:    Dual license model; choose one of two:
             New BSD License
             http://www.opensource.org/licenses/bsd-license.php
             - or -
             GNU Lesser General Public License, version 2.1
             http://www.gnu.org/licenses/lgpl-2.1.html

 Copyright 2015 Richard Bateman and the FireBreath Dev Team
\**********************************************************/

#pragma once
#ifndef H_FBDEFERREDUTILS
#define H_FBDEFERREDUTILS

#include <Deferred.h>

namespace FB {
    variantPromise whenAllPromises(VariantPromiseList args, std::function<variantPromise(VariantList)> cb);
}

#endif // H_FBDEFERREDUTILS