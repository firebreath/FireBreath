/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Nov 24, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_PLUGINWINDOWWIN
#define H_PLUGINWINDOWWIN

#include "Win/win_common.h"
#include "PluginWindow.h"
#include "WindowContextWin.h"

#include <map>

#define WM_ASYNCTHREADINVOKE    WM_USER + 1

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  PluginWindowWin
    ///
    /// @brief  Windows specific implementation of PluginWindow
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class PluginWindowWin : public PluginWindow
    {
    public:
        PluginWindowWin(const WindowContextWin&);
        virtual ~PluginWindowWin();
        static LRESULT CALLBACK _WinProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                                          LPARAM lParam);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn HWND PluginWindowWin::getHWND()
        ///
        /// @brief  Gets the HWND of the plugin window
        ///
        /// @note   You *must not* attempt to use this HWND after DetachedEvent is fired.
        /// @return The HWND
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        HWND getHWND() { return m_hWnd; }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void PluginWindowWin::setBrowserHWND(HWND hWnd)
        ///
        /// @brief  Called by the browser to store the HWND of the browser window
        ///
        /// @param  hWnd    Handle of the browser window. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void setBrowserHWND(HWND hWnd) { m_browserhWnd = hWnd; }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn HWND PluginWindowWin::getBrowserHWND()
        ///
        /// @brief  Gets the browser window HWND 
        ///
        /// @return The browser HWND 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        HWND getBrowserHWND() { return m_browserhWnd; }
        void setCallOldWinProc(bool callOld) { m_callOldWinProc = callOld; }

        typedef std::map<void*,PluginWindowWin*> PluginWindowMap;

        virtual void InvalidateWindow();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static HWND PluginWindowWin::createMessageWindow()
        ///
        /// @brief  Creates a message window. Don't touch this if you don't understand what you are doing
        ///
        /// @return The handle of the window. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static HWND createMessageWindow();

    protected:
        static PluginWindowMap m_windowMap;

        bool WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParamm, LRESULT & lRes);
        WNDPROC lpOldWinProc;
        bool m_callOldWinProc;
        HWND m_hWnd;
        HWND m_browserhWnd;

        virtual bool CustomWinProc(HWND hWnd,
                                   UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParamm,
                                   LRESULT & lRes) { return false; }
    };

};

#endif // H_PLUGINWINDOWWIN
