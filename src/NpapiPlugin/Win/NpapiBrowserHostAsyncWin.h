
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

#ifndef H_NPAPIBROWSERHOSTASYNCWIN
#define H_NPAPIBROWSERHOSTASYNCWIN

#include "../NpapiBrowserHost.h"

namespace FB { namespace Npapi 
{
    class NpapiBrowserHostAsyncWin : public NpapiBrowserHost
    {
    public:
        NpapiBrowserHostAsyncWin(NpapiPluginModule *module, NPP npp);
        virtual ~NpapiBrowserHostAsyncWin();

        virtual void ScheduleAsyncCall(void (*func)(void*), void* userData);
        void setWindow(NPWindow* window);

    private:
        HWND m_hwnd;
    };
} }
#endif