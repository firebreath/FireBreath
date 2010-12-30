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

#include <boost/shared_ptr.hpp>

struct IDispatchEx;
typedef long HRESULT;

namespace FB {
    class BrowserHost;
    typedef boost::shared_ptr<BrowserHost> BrowserHostPtr;
}

IDispatchEx* _getCOMJSWrapper(FB::BrowserHostPtr host, FB::JSAPIPtr api);
HRESULT _updateRegistry( bool res );

#endif // H_NpapiFactoryDefinitions

