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

#include "TimerService.h"

using namespace FB;

TimerService * TimerService::inst = NULL;
boost::mutex TimerService::instance_mutex;

TimerService* TimerService::instance()
{
	boost::mutex::scoped_lock lock(instance_mutex);
	if(inst == NULL)
	{
		inst = new TimerService();
	}
	return inst;
}

TimerService::TimerService()
: io_service(new boost::asio::io_service()),
io_idlework(new boost::asio::io_service::work(*io_service)),
io_thread(NULL)
{
	io_thread.reset(new boost::thread(
		boost::bind(&boost::asio::io_service::run, TimerService::io_service.get())));
}

TimerService::~TimerService()
{
	io_service->stop();
	io_thread->join();
	io_idlework.reset();
	io_thread.reset();
	io_service.reset();
}

boost::asio::io_service* TimerService::getIOService()
{
	return io_service.get();
}
