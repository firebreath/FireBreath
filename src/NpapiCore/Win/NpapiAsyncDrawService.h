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
#ifndef H_NPAPIASYNCDRAWSERVICE
#define H_NPAPIASYNCDRAWSERVICE

#include "NpapiBrowserHost.h"
#include "Win\D3d10AsyncDrawService.h"

namespace FB {
    namespace Npapi {

        FB_FORWARD_PTR(NpapiAsyncDrawService);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @class  NpapiAsyncDrawService
        ///
        /// @brief  Provides a D3d10AsyncDrawService implementation for NPAPI
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class NpapiAsyncDrawService : public D3d10AsyncDrawService
        {
            // wrap the NPAsyncSurface to track initialised state
            struct Surface : public NPAsyncSurface
            {
                Surface();
                NPError init(NpapiBrowserHost*, NPSize*);
                void finalize(NpapiBrowserHost*);
                void makeCurrent(NpapiBrowserHost*);
                bool sizeIsDifferent(unsigned, unsigned) const;
                bool valid;
            };

            static void finalizeSurfaces(NpapiBrowserHostWeakPtr weakHost, Surface a, Surface b);

            NpapiBrowserHostWeakPtr m_weakHost;
            Surface m_surface[2];
            int m_current;

            virtual void present(bool resizing);

        public:
            NpapiAsyncDrawService(NpapiBrowserHostPtr host);
            ~NpapiAsyncDrawService();
        };
    };
};

#endif
