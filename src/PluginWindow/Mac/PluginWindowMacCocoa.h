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

    class PluginWindowMacCocoa : public PluginWindow {
        public:
            PluginWindowMacCocoa();
            virtual ~PluginWindowMacCocoa();

            virtual int16_t HandleEvent(NPCocoaEvent* evt);
            virtual void InvalidateWindow();
            
            // Set this window's browser host
            void setNpHost(FB::Npapi::NpapiBrowserHostPtr host) { m_npHost = host; }

            // Schedules a timer, returns the timer's unique ID
            virtual int scheduleTimer(int interval, bool repeat);
            // Unschedules timer corresponding to unique ID
            virtual void unscheduleTimer(int timerId);
            // Handles a timer event 
            virtual void handleTimerEvent();

            virtual NPRect getWindowPosition();
            virtual NPRect getWindowClipping();
            virtual int getWindowHeight();
            virtual int getWindowWidth();
            virtual void setWindowPosition(int32_t x, int32_t y, int32_t width, int32_t height);
            virtual void setWindowClipping(uint16_t top, uint16_t left, uint16_t bottom, uint16_t right);

        protected:
            int m_x, m_y, m_width, m_height;
            int m_clipTop, m_clipLeft, m_clipBottom, m_clipRight;
            Npapi::NpapiBrowserHostPtr m_npHost;
    };
};

#endif // H_PLUGINWINDOWMAC_COCOA
