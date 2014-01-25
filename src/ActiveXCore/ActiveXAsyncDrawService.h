/**********************************************************\ 
Original Author: Gil Gonen

Created:    Jan 28, 2013
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2013 Gil Gonen and the Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_ACTIVEXASYNCDRAWSERVICE
#define H_ACTIVEXASYNCDRAWSERVICE

#include <atlbase.h>
#include <mshtml.h>
#include "APITypes.h"
#include "AsyncDrawService.h"
#include "Win\D3d10AsyncDrawService.h"

namespace FB {
    namespace ActiveX {

        FB_FORWARD_PTR(ActiveXAsyncDrawService);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @class  ActiveXAsyncDrawService
        ///
        /// @brief  Provides a D3d10AsyncDrawService implementation for ActiveX
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class ActiveXAsyncDrawService : public D3d10AsyncDrawService
        {
        public:
            ActiveXAsyncDrawService(FB::BrowserHostPtr host, IViewObjectPresentSite*);

        private:
            void present(bool init);

            CComPtr<IViewObjectPresentSite> m_pViewObjectPresentSite;
            CComPtr<ISurfacePresenter> m_pSurfacePresenter;
        };
    }
}
#endif
