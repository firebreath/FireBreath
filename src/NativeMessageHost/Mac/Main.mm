/**********************************************************\
Original Author: Wojciech Walek

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

#include "Main.h"
#import <Cocoa/Cocoa.h>

void runLoop(int miliseconds)
{
	float seconds = miliseconds / 1000.0;
	@autoreleasepool {
		NSDate *loopUntil = [NSDate dateWithTimeIntervalSinceNow:seconds];

		[[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode beforeDate:loopUntil];
	}
}