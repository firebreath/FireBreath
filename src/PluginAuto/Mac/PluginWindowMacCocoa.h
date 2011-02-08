/**********************************************************\
Original Author: Anson MacKeracher 

Created:    Mar 26, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Anson MacKeracher, Firebreath development team
\**********************************************************/

#ifndef H_PLUGINWINDOWMAC_COCOA
#define H_PLUGINWINDOWMAC_COCOA

#include "PluginWindow.h"
#include "KeyCodesCocoa.h"
#include "NpapiTypes.h"
#include <map>
#include "NpapiBrowserHost.h"
#include "Mac/NpapiPluginMac.h"
#include "NpapiPluginModule.h"

#include "PluginEvents/MacEventCocoa.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h"
#include "PluginEvents/KeyboardEvents.h"

namespace FB {

    void timerCallback(NPP npp, uint32_t timerID);
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  PluginWindowMacCocoa
    ///
    /// @brief  Mac OS X Cocoa specific implementation of PluginWindow
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class PluginWindowMacCocoa : public PluginWindow {
	public:
		PluginWindowMacCocoa();
		virtual ~PluginWindowMacCocoa();

		virtual int16_t HandleEvent(NPCocoaEvent* evt);
		virtual void InvalidateWindow() const;
		
		// Set this window's browser host
		void setNpHost(FB::Npapi::NpapiBrowserHostPtr host) { m_npHost = host; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @fn     int PluginWindowMacCocoa::scheduleTimer(int interval, bool repeat)
		///
		/// @brief  Schedule a Cocoa timer to notify you on the given interval or period
		/// 
		/// @param  interval    Numer of milliseconds before the timer event is first called
		/// @param  repeat      If true, timer will repeat (at a period specified by the interval parameter)
		////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual int scheduleTimer(int interval, bool repeat);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @fn     int PluginWindowMacCocoa::unscheduleTimer(int timerId)
		///
		/// @brief  Unschedule a previously scheduled timer
		/// 
		/// @param  timerId     The id of the previously scheduled timer
		////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual void unscheduleTimer(int timerId);
		// Handles a timer event 
		virtual void handleTimerEvent();

		virtual FB::Rect getWindowPosition() const;
		virtual FB::Rect getWindowClipping() const;
		virtual uint32_t getWindowHeight() const;
		virtual uint32_t getWindowWidth() const;
		virtual void setWindowPosition(int32_t x, int32_t y, int32_t width, int32_t height);
		virtual void setWindowClipping(uint32_t top, uint32_t left, uint32_t bottom, uint32_t right);

	protected:
		int m_x, m_y, m_width, m_height;
		int m_clipTop, m_clipLeft, m_clipBottom, m_clipRight;
		Npapi::NpapiBrowserHostPtr m_npHost;
    };
};

#endif // H_PLUGINWINDOWMAC_COCOA

