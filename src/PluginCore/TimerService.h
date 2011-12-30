/**********************************************************\
Original Author: Mital Vora <mital.d.vora@gmail.com>

Created:    Apr 03, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
\**********************************************************/

#pragma once
#ifndef H_FB_TIMER_SERVICE
#define H_FB_TIMER_SERVICE

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/scoped_ptr.hpp>

#include "FBPointers.h"

namespace boost { namespace asio { class io_service; } }

namespace FB {

    class TimerServicePimpl;
    FB_FORWARD_PTR(TimerService);
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  TimerService
    ///
    /// @brief  TimerService Utility.
    ///
    /// Timer Utility handles io_service and separate thread for Timer.
    ///
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
	class TimerService
	{
	public:
		static TimerServicePtr instance();
		~TimerService();

		boost::asio::io_service* getIOService();
	protected:
		static TimerServiceWeakPtr inst;
		static boost::mutex instance_mutex;
		TimerService();

	private:
        boost::scoped_ptr<TimerServicePimpl> pimpl;
	};
};

#endif

