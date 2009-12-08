
#include "PluginEvents/WindowsEvent.h"
#include "PluginWindowWin.h"
    
#include "windows.h"
#include "windowsx.h"

using namespace FB;

PluginWindowWin::PluginWindowMap FB::PluginWindowWin::m_windowMap;

PluginWindowWin::PluginWindowWin(HWND hWnd) : m_hWnd(hWnd), lpOldWinProc(NULL)
{
    // subclass window so we can intercept window messages 
    lpOldWinProc = SubclassWindow(m_hWnd, (WNDPROC)PluginWindowWin::_WinProc);

    // associate window with this object so that we can route events properly
    m_windowMap[static_cast<void*>(m_hWnd)] = this;
}

PluginWindowWin::~PluginWindowWin()
{
    // Unsubclass the window so that everything is as it was before we got it
	SubclassWindow(m_hWnd, lpOldWinProc);

    PluginWindowMap::iterator it = m_windowMap.find(static_cast<void*>(m_hWnd));
    if (it != m_windowMap.end()) 
        m_windowMap.erase(it);
}

bool PluginWindowWin::WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT lRes)
{

    WindowsEvent evt(hWnd, uMsg, wParam, lParam);
    return this->SendEvent(&evt);
}

LRESULT CALLBACK PluginWindowWin::_WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PluginWindowMap::iterator it = m_windowMap.find(static_cast<void*>(hWnd));
    if (it == m_windowMap.end()) 
        return 0;
	PluginWindowWin *win = it->second;

	LRESULT lResult(0);
	// Try to handle the event through the plugin instace; if that doesn't work, handle it through the default winproc
	if (win->WinProc(hWnd, uMsg, wParam, lParam, lResult))
		return lResult;
	else
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
