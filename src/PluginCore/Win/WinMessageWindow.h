#pragma once
#ifndef WinMessageWindow_h__
#define WinMessageWindow_h__

#include <boost/noncopyable.hpp>

namespace FB {
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
    private:
        HWND m_hWnd;
    };
}
#endif // WinMessageWindow_h__

