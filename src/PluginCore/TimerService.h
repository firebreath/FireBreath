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

#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

namespace FB {

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
		static TimerService* instance();
		
		boost::asio::io_service* getIOService();
	protected:
		static TimerService * inst;
		static boost::mutex instance_mutex;
		TimerService();
		~TimerService();

	private:
		std::auto_ptr<boost::asio::io_service> io_service;
		std::auto_ptr<boost::asio::io_service::work> io_idlework;
		std::auto_ptr<boost::thread> io_thread;
	};
};

#endif

