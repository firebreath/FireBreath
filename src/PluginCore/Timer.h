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
#ifndef H_FB_TIMER
#define H_FB_TIMER

#include <string>
#include <boost/function.hpp>

#include "FBPointers.h"
#include "TimerService.h"

namespace FB {

	FB_FORWARD_PTR(Timer);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  Timer
    ///
    /// @brief  Timer Utility.
    ///         
    /// Timer Utility class which helps setting async callbacks to the same thread.
    /// 
    /// 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Timer
    {
		typedef boost::function<void (void)> TimerCallbackFunc;

	private:
		boost::asio::deadline_timer timer;
		const long duration;
		const bool recursive;
		TimerCallbackFunc cb;

		Timer(long _duration, bool _recursive, TimerCallbackFunc _callback);
		void callback(const boost::system::error_code& error);

	public:
        ~Timer();

		void start();
		bool stop();

		static TimerPtr getTimer(long _duration, bool _recursive, TimerCallbackFunc _callback);
    };
};

#endif

