/**********************************************************\
Original Author: Eric Herrmann 

Created:    Feb 8, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2011 Eric Herrmann, Firebreath development team
\**********************************************************/

#ifndef H_PLUGINWINDOWMAC
#define H_PLUGINWINDOWMAC

#include "NpapiBrowserHost.h"
#include "NpapiTypes.h"
#include "FBPointers.h"

#include "PluginWindow.h"

namespace FB {

	FB_FORWARD_PTR(PluginEventMac);
	FB_FORWARD_PTR(PluginWindowMac);

    class PluginWindowMac : public PluginWindow
    {
    public:        
        enum DrawingModel
        {
            DrawingModelQuickDraw,
            DrawingModelCoreGraphics,
            DrawingModelCoreAnimation,
            DrawingModelInvalidatingCoreAnimation
        };

		static NPDrawingModel initPluginWindowMac(const FB::Npapi::NpapiBrowserHostPtr &host);
		static FB::PluginWindowMac* createPluginWindowMac(NPDrawingModel drawingModel);

        PluginWindowMac();
        virtual ~PluginWindowMac() {}

		virtual NPError SetWindow(NPWindow* window);

        virtual DrawingModel getDrawingModel() const = 0;
		virtual void* getDrawingPrimitive() const = 0;
		virtual WindowRef getWindowRef() const = 0;
		
		void setNpHost(FB::Npapi::NpapiBrowserHostPtr host) { m_npHost = host; }
		void setPluginEvent(PluginEventMacPtr event) { m_PluginEvent = event; }
		PluginEventMacPtr getPluginEvent() { return m_PluginEvent.lock(); }

        FB::Rect getWindowPosition() const;
        uint32_t getWindowWidth() const;
        uint32_t getWindowHeight() const;
        FB::Rect getWindowClipping() const;

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @fn     int PluginWindowMacCocoa::scheduleTimer(int interval, bool repeat)
		///
		/// @brief  Schedule a Cocoa timer to notify you on the given interval or period
		/// 
		/// @param  interval    Numer of milliseconds before the timer event is first called
		/// @param  repeat      If true, timer will repeat (at a period specified by the interval parameter)
		////////////////////////////////////////////////////////////////////////////////////////////////////
		int scheduleTimer(int interval, bool repeat);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @fn     int PluginWindowMacCocoa::unscheduleTimer(int timerId)
		///
		/// @brief  Unschedule a previously scheduled timer
		/// 
		/// @param  timerId     The id of the previously scheduled timer
		////////////////////////////////////////////////////////////////////////////////////////////////////
		void unscheduleTimer(int timerId);
		// Handles a timer event 
		void handleTimerEvent();

    protected:
        Npapi::NpapiBrowserHostPtr m_npHost;
        PluginEventMacWeakPtr m_PluginEvent;
		
        int32_t m_x;
        int32_t m_y;
        uint32_t m_width;
        uint32_t m_height;
        
        int32_t m_clipTop;
        int32_t m_clipLeft;
        int32_t m_clipBottom;
        int32_t m_clipRight;
    };
};

#endif // H_PLUGINWINDOWMAC
