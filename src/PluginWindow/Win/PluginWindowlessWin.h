#ifndef H_PLUGINWINDOWLESSWIN
#define H_PLUGINWINDOWLESSWIN

/**********************************************************\ 
Original Author: Anson MacKeracher (amackera) 

Created:    Aug 5, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Anson MacKeracher, Firebreath development team
\**********************************************************/

#include "Win/win_common.h"
#include "PluginWindow.h"

#include "NpapiBrowserHost.h"
#include "NpapiPlugin.h"
#include "Win/KeyCodesWin.h"

#include "PluginEvents/WindowsEvent.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h" 
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/KeyboardEvents.h"

/* This class implements the windowless plugin "window" in FireBreath.
 * Windowless plugins are different from windowed plugins in that they
 * don't have a handle to a window, and instead are given a handle to
 * a drawable from the browser. Also all plugin events are delivered
 * through the NPAPI NPP_HandleEvent() function, instead of intercepting
 * the events directly through the window.
 */

namespace FB {
    class PluginWindowlessWin : public PluginWindow
    {
        public:
            PluginWindowlessWin(HDC hdc);
            ~PluginWindowlessWin();
            
            // HDC is the "Handle to Device Context" given to us in SetWindow
            HDC getHDC() { return m_hdc; }
            void setHDC(HDC hdc) { m_hdc = hdc; }
            HWND getHWND(); 

            // NpapiBrowserHost is used to send calls to NPAPI
            Npapi::NpapiBrowserHostPtr getNpHost() { return m_npHost; }
            void setNpHost(Npapi::NpapiBrowserHostPtr npHost) { m_npHost = npHost; }

            // Returns the plugin's position within the page
            NPRect getWindowPosition();
            void setWindowPosition(int x, int y, int width, int height);
            void setWindowPosition(NPRect pos);

            // Returns the clipping rect that the plugin is clipped to in the page
            NPRect getWindowClipping();
            void setWindowClipping(int top, int left, int bottom, int right);
            void setWindowClipping(NPRect clip);

            int getWindowWidth() { return m_width; }
            int getWindowHeight() { return m_height; }

            // Converts window-space coordinates into plugin-place coordinates
            void translateWindowToPlugin(int &x, int &y);

            // Handle event given to us from NPAPI (windowless plugins don't intercept raw Windows events)
            int16_t HandleEvent(NPEvent* evt);

            // Invalidate the window from *the plugin's* thread
            void InvalidateWindow();
            // Invalidate the window from any thread (useful for drawing)
            void AsyncInvalidateWindow();

        protected:
            HDC m_hdc;
            HWND m_browserHwnd;
            Npapi::NpapiBrowserHostPtr m_npHost;
            int m_x, m_y, m_width, m_height; 
            int m_clipTop, m_clipLeft, m_clipBottom, m_clipRight;
    };    
};

#endif
