#pragma once
#ifndef H_ActiveXFactoryDefinitions
#define H_ActiveXFactoryDefinitions
/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sep 11, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Richard Bateman, Firebreath development team
\**********************************************************/

#include "FBPointers.h"
#include "ShareableReference.h"

struct IDispatchEx;
typedef long HRESULT;

namespace FB {
    FB_FORWARD_PTR(BrowserHost);
    FB_FORWARD_PTR(JSAPI);
    typedef boost::weak_ptr<FB::ShareableReference<IDispatchEx> > WeakIDispatchExRef;
}

IDispatchEx* _getCOMJSWrapper(const FB::BrowserHostPtr& host, const FB::JSAPIWeakPtr& api, bool autoRelease = false);
const FB::WeakIDispatchExRef _getWeakRefFromCOMJSWrapper(IDispatchEx* wrapper);

HRESULT _updateRegistry( bool res );

#endif // H_NpapiFactoryDefinitions

