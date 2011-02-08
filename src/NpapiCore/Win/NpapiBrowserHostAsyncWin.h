
/**********************************************************\
Original Author: Georg Fritzsche

Created:    March 1, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Georg Fritzsche, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_NPAPIBROWSERHOSTASYNCWIN
#define H_NPAPIBROWSERHOSTASYNCWIN

#include "NpapiBrowserHost.h"
#include <boost/scoped_ptr.hpp>

namespace FB { 
    class WinMessageWindow;
    namespace Npapi {
        class NpapiBrowserHostAsyncWin : public NpapiBrowserHost
        {
        public:
            NpapiBrowserHostAsyncWin(NpapiPluginModule *module, NPP npp);
            virtual ~NpapiBrowserHostAsyncWin();

            virtual bool _scheduleAsyncCall(void (*func)(void*), void* userData) const;

        private:
            boost::scoped_ptr<FB::WinMessageWindow> m_messageWin;
        };
    }
}
#endif

