/**********************************************************\ 
Original Author: Mital Vora <mital.d.vora@gmail.com>

Created:    Mar 26, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
\**********************************************************/

#pragma once
#ifndef H_FB_TIMER_WIN32
#define H_FB_TIMER_WIN32

#include <string>
#include <windows.h>
#include "Timer.h"

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  WinTimer
    ///
    /// @brief  Windows Timer Utility 
    ///         
    /// Windows Timer Utility class which helps setting async callbacks to the same thread.
    /// 
    /// 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
	class WinTimer : public Timer
    {
	private:
		unsigned loadTimeTimerID;

	public:
        WinTimer(long _duration, bool _recursive, TimerCallbackFunc _callback);
        virtual ~WinTimer();

		virtual void start();
		virtual bool stop();
    };
};

#endif

