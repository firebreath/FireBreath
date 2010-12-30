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

#include "win_common.h"
#include "PluginWindow.h"

#include "NpapiBrowserHost.h"
#include "NpapiPlugin.h"
#include "Win/KeyCodesWin.h"

#include "Win/WindowContextWin.h"

/* This class implements the windowless plugin "window" in FireBreath.
 * Windowless plugins are different from windowed plugins in that they
 * don't have a handle to a window, and instead are given a handle to
 * a drawable from the browser. Also all plugin events are delivered
 * through the NPAPI NPP_HandleEvent() function, instead of intercepting
 * the events directly through the window.
 */

namespace FB {
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  PluginWindowlessWin
    ///
    /// @brief  Windows specific implementation of PluginWindow. Uses NPAPI windowless mode.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class PluginWindowlessWin : public PluginWindow
    {
        public:
            PluginWindowlessWin(const WindowContextWindowless&);
            ~PluginWindowlessWin();
            
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @fn HDC PluginWindowlessWin::getHDC()
            ///
            /// @brief  Gets the HDC of the plugin window
            ///
            /// @note   The window's HDC is likely to change frequently
            /// @return The HDC 
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            HDC getHDC() { return m_hdc; }
            void setHDC(HDC hdc) { m_hdc = hdc; }
            HWND getHWND(); 

            // NpapiBrowserHost is used to send calls to NPAPI
            Npapi::NpapiBrowserHostPtr getNpHost() { return m_npHost; }
            void setNpHost(Npapi::NpapiBrowserHostPtr npHost) { m_npHost = npHost; }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @fn NPRect PluginWindowlessWin::getWindowPosition()
            ///
            /// @brief  Gets the position rect of the window
            ///
            /// @return The position rect of the window
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            NPRect getWindowPosition();
            void setWindowPosition(int x, int y, int width, int height);
            void setWindowPosition(NPRect pos);

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @fn NPRect PluginWindowlessWin::getWindowClipping()
            ///
            /// @brief  Gets the clipping rect of the window
            ///
            /// @return The clipping rect of the window
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            NPRect getWindowClipping();
            void setWindowClipping(int top, int left, int bottom, int right);
            void setWindowClipping(NPRect clip);

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @fn int PluginWindowlessWin::getWindowWidth()
            ///
            /// @brief  Gets the width of the window
            ///
            /// @return The width of the window
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            int getWindowWidth() { return m_width; }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @fn int PluginWindowlessWin::getWindowHeight()
            ///
            /// @brief  Gets the height of the window
            ///
            /// @return The height of the window
            ////////////////////////////////////////////////////////////////////////////////////////////////////
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
