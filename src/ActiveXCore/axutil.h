/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    Sept 17, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_AXUTIL
#define H_AXUTIL

#include "win_common.h"
#include "TypeIDMap.h"

namespace FB { 
    namespace ActiveX {
        extern FB::TypeIDMap<DISPID> AxIdMap;

        class FbPerUserRegistration
        {
        public:
            FbPerUserRegistration(bool perUser = true);
            ~FbPerUserRegistration();

        private:
        #if _ATL_VER < 0x0900
            static void EnablePerUserTLibRegistration();
            bool m_mapping;
        #endif
        };
    }
}

#endif

