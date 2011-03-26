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
		static unsigned timerid;

	protected:

		typedef boost::function<void (void)> TimerCallbackFunc;

		const unsigned id;
		const long duration;
		const bool recursive;
		TimerCallbackFunc cb;

		Timer(long _duration, bool _recursive, TimerCallbackFunc _callback);

	public:
        virtual ~Timer() { };

		virtual void start()=0;
		virtual bool stop()=0;
		void callback();

		unsigned getID() const { return id; }

		static TimerPtr getTimer(long _duration, bool _recursive, TimerCallbackFunc _callback);
    };
};

#endif

