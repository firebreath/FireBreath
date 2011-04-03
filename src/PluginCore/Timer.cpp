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

#include "Timer.h"

using namespace FB;

TimerPtr Timer::getTimer(long _duration, bool _recursive, TimerCallbackFunc _callback)
{
	return boost::shared_ptr<FB::Timer>(new Timer(_duration, _recursive, _callback));
}

Timer::Timer(long _duration, bool _recursive, TimerCallbackFunc _callback)
	: duration(_duration),
	recursive(_recursive),
	cb(_callback),
	timer(*(TimerService::instance()->getIOService()))
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
	timer.expires_from_now(boost::posix_time::milliseconds(duration));
	timer.async_wait(boost::bind(&Timer::callback, this, boost::asio::placeholders::error));
}
bool Timer::stop()
{
	timer.cancel();
	return false;
}

