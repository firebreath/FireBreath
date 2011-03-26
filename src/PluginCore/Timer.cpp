/**********************************************************\ 
Original Author: Mital Vora <mital.d.vora@gmail.com>

Created:    Mar 26, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#include "Timer.h"

using namespace FB;

unsigned Timer::timerid = 0;

Timer::Timer(long _duration, bool _recursive, TimerCallbackFunc _callback)
	: id(++Timer::timerid),
	duration(_duration),
	recursive(_recursive),
	cb(_callback)
{
}

void Timer::callback()
{
	if (! this->recursive)
	{
		this->stop();
	}
	if (cb) cb();
}