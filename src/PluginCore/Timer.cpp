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

#ifdef FB_WIN
#include "win_targetver.h"
#endif
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
	return std::shared_ptr<FB::Timer>(new Timer(_duration, _recursive, _callback));
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
    auto self = shared_from_this();
	pimpl->timer.expires_from_now(boost::posix_time::milliseconds(duration));
    pimpl->timer.async_wait([self](const boost::system::error_code& error) { self->callback(error); });
}
bool Timer::stop()
{
	pimpl->timer.cancel();
	return false;
}

