
#include "PluginEvents/WindowsEvent.h"
#include "PluginWindowWin.h"
    
#include "windows.h"
#include "windowsx.h"

using namespace FB;

PluginWindowWin::PluginWindowWin(HWND hWnd) : m_hWnd(hWnd)
{
    // subclass window so we can intercept window messages 
    //lpOldWinProc = SubclassWindow(m_hWnd, (WNDPROC)PluginWindowWin::_WinProc);

    // associate window with this object so that we can route events properly
    //SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
}

PluginWindowWin::~PluginWindowWin()
{
    // Unsubclass the window so that everything is as it was before we got it
	//SubclassWindow(m_hWnd, lpOldWinProc);
}

bool PluginWindowWin::WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT lRes)
{

    WindowsEvent evt(hWnd, uMsg, wParam, lParam);
    return this->SendEvent(&evt);
}

LRESULT CALLBACK PluginWindowWin::_WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PluginWindowWin *win = (PluginWindowWin *)(GetWindowLong(hWnd, GWL_USERDATA));

	LRESULT lResult(0);
	// Try to handle the event through the plugin instace; if that doesn't work, handle it through the default winproc
	if (win->WinProc(hWnd, uMsg, wParam, lParam, lResult))
		return lResult;
	else
		return win->lpOldWinProc(hWnd, uMsg, wParam, lParam);
}
