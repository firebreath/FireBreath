/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Jan 5, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Richard Bateman, Firebreath development team
\**********************************************************/
#pragma once
#ifndef WinMessageWindow_h__
#define WinMessageWindow_h__

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace FB {
    typedef boost::function<bool (HWND, UINT, WPARAM, LPARAM, LRESULT&)> WinProcHandler;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class WinMessageWindow
    ///
    /// @brief  Creates a message window. Don't touch this if you don't understand what you are doing
    ///
    /// @return The handle of the window. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class WinMessageWindow : boost::noncopyable
    {
    public:
        WinMessageWindow();
        ~WinMessageWindow();

        HWND getHWND();

        static LRESULT CALLBACK _WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        bool DefaultWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
        void setWinProc( const WinProcHandler& handler ) { winProc = handler; }
    private:
        HWND m_hWnd;
        WinProcHandler winProc;
    };
}
#endif // WinMessageWindow_h__

