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

#include "precompiled_headers.h" // On windows, everything above this line in PCH
#include <boost/asio.hpp>
#include "Timer.h"
#include "TimerService.h"

using namespace FB;

namespace FB {
    class TimerPimpl {
    public:
        TimerPimpl() : timerService(TimerService::instance()), timer(*timerService->getIOService()) {
        }

        TimerServicePtr timerService;
		boost::asio::deadline_timer timer;
    };
};

TimerPtr Timer::getTimer(long _duration, bool _recursive, TimerCallbackFunc _callback)
{
	return boost::shared_ptr<FB::Timer>(new Timer(_duration, _recursive, _callback));
}

Timer::Timer(long _duration, bool _recursive, TimerCallbackFunc _callback)
	: duration(_duration),
	recursive(_recursive),
	cb(_callback), pimpl(new TimerPimpl)
{
}

Timer::~Timer()
{
	this->stop();
}


void Timer::callback(const boost::system::error_code& error)
{
	if (error)
	{
		if (error == boost::asio::error::operation_aborted)
		{
			return;
		}
		// TODO: prompt timer Error.
		return;
	}

	if (this->recursive)
	{
		this->start();
	}
	if (cb) cb();
}

void Timer::start()
{
	pimpl->timer.expires_from_now(boost::posix_time::milliseconds(duration));
	pimpl->timer.async_wait(boost::bind(&Timer::callback, this, boost::asio::placeholders::error));
}
bool Timer::stop()
{
	pimpl->timer.cancel();
	return false;
}

