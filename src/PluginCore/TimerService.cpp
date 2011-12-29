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

#include "win_targetver.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH
#include "TimerService.h"
#include <boost/asio.hpp>

using namespace FB;

namespace FB {
    class TimerServicePimpl {
    public:
        TimerServicePimpl() :
            io_service(new boost::asio::io_service()),
            io_idlework(new boost::asio::io_service::work(*io_service)),
            io_thread(NULL) {}

        ~TimerServicePimpl() {
            io_service->stop();
            io_thread->join();
            io_idlework.reset();
            io_thread.reset();
            io_service.reset();
        }

        boost::scoped_ptr<boost::asio::io_service> io_service;
        boost::scoped_ptr<boost::asio::io_service::work> io_idlework;
        boost::scoped_ptr<boost::thread> io_thread;
    };
};

TimerServiceWeakPtr TimerService::inst;
boost::mutex TimerService::instance_mutex;

TimerServicePtr TimerService::instance()
{
	boost::mutex::scoped_lock lock(instance_mutex);
	TimerServicePtr service(inst.lock());
    if(!service)
	{
		service = TimerServicePtr(new TimerService());
        inst = service;
	}
	return service;
}

TimerService::TimerService() : pimpl(new TimerServicePimpl)
{
	pimpl->io_thread.reset(new boost::thread(
		boost::bind(&boost::asio::io_service::run, pimpl->io_service.get())));
}

TimerService::~TimerService()
{
}

boost::asio::io_service* TimerService::getIOService()
{
	return pimpl->io_service.get();
}
