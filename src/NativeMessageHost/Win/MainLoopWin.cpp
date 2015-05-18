/**********************************************************\
Original Author: Richard Bateman

Created:    May 18, 2015
License:    Dual license model; choose one of two:
New BSD License
http://www.opensource.org/licenses/bsd-license.php
- or -
GNU Lesser General Public License, version 2.1
http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 GradeCam, Richard Bateman, and the
Firebreath development team
\**********************************************************/

#include "MainLoop.h"

void MainLoop::waitForWork(std::unique_lock<std::mutex> *_l)
{
	auto workExists = [this]() {
		return m_needsToExit || m_messagesIn.size() || m_AsyncCalls.size();
	};

	// If work already exists, don't wait
	if (!workExists()) {
		m_cond.wait(*_l, workExists);
	}
}