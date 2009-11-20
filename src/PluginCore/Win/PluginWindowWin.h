
#ifndef H_PLUGINWINDOWWIN
#define H_PLUGINWINDOWWIN

#include "Win/win_common.h"
#include "PluginWindow.h"

namespace FB {

    class PluginWindowWin : public PluginWindow
    {
    public:
        PluginWindowWin(HWND hWnd);
        virtual ~PluginWindowWin();
        static LRESULT CALLBACK _WinProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                                          LPARAM lParam);

    protected:
        bool WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParamm, LRESULT lRes);
    	WNDPROC lpOldWinProc;
        HWND m_hWnd;
    };

};

#endif // H_PLUGINWINDOWWIN