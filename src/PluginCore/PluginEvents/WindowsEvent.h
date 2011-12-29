/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_EVENTS_WINDOWSEVENT
#define H_FB_EVENTS_WINDOWSEVENT

#ifndef _WINDOWS
#define HWND void *
#else
#include "win_common.h"
#endif

#include "PluginEvent.h"

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  WindowsEvent
    ///
    /// @brief  Generic windows event.  All windows events going through the winproc get first fired
    ///         as a WindowEvent and then fired as the specific type, allowing you to handle them
    ///         either way
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class WindowsEvent : public PluginEvent
    {
    public:
        WindowsEvent(HWND hWnd, uint32_t msg, uint32_t wparam, long lparam, LRESULT & lres)
            : hWnd(hWnd), uMsg(msg), wParam(wparam), lParam(lparam), lRes(lres)
        { }

    public:
        HWND hWnd;
        uint32_t uMsg;
        uint32_t wParam;
        long lParam;
        LRESULT & lRes;
    };
};

#endif

