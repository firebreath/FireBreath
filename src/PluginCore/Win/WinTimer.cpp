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

#include <map>
#include "WinTimer.h"

using namespace FB;

TimerPtr Timer::getTimer(long _duration, bool _recursive, TimerCallbackFunc _callback)
{
	return boost::shared_ptr<FB::Timer>(new WinTimer(_duration, _recursive, _callback));
}

std::map<unsigned, Timer *> timers;

WinTimer::WinTimer(long _duration, bool _recursive, TimerCallbackFunc _callback)
	: Timer(_duration, _recursive, _callback),
	loadTimeTimerID(0)
{
}

WinTimer::~WinTimer()
{
	if (this->loadTimeTimerID != 0)
	{
		std::map<unsigned, Timer *>::iterator oIter = timers.find(this->loadTimeTimerID);
		if(oIter != timers.end())
		{
			timers.erase(oIter);
		}
	}
}

VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	std::map<unsigned, Timer *>::iterator oIter = timers.find(idEvent);
	if(oIter != timers.end())
	{
		Timer * timer = oIter->second;
		timer->callback();
	}
}

void WinTimer::start()
{
	this->loadTimeTimerID = SetTimer(NULL, this->id, this->duration, &TimerProc);
	timers[this->loadTimeTimerID] = this;
}

bool WinTimer::stop()
{
	if (this->loadTimeTimerID)
	{
		KillTimer(0 , this->loadTimeTimerID);
		std::map<unsigned, Timer *>::iterator oIter = timers.find(this->loadTimeTimerID);
		if(oIter != timers.end())
		{
			timers.erase(oIter);
			return true;
		}
	}
	return false;
}
