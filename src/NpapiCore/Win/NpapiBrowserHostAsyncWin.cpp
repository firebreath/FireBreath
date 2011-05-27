
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

#include "precompiled_headers.h" // On windows, everything above this line in PCH
#include "AsyncFunctionCall.h"
#include "Win/WinMessageWindow.h"

#include "NpapiBrowserHostAsyncWin.h"
using namespace FB::Npapi;

NpapiBrowserHostAsyncWin::NpapiBrowserHostAsyncWin(NpapiPluginModule* module, NPP npp)
  : NpapiBrowserHost(module, npp)
  , m_messageWin(new FB::WinMessageWindow())
{

}

NpapiBrowserHostAsyncWin::~NpapiBrowserHostAsyncWin()
{

}

bool NpapiBrowserHostAsyncWin::_scheduleAsyncCall(void (*func)(void*), void* userData) const
{
    return ::PostMessage(m_messageWin->getHWND(), WM_ASYNCTHREADINVOKE, NULL, 
                  (LPARAM)new FB::AsyncFunctionCall(func, userData)) ? true : false;
}

