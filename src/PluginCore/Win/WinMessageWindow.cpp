
#include "win_targetver.h"
#include <windows.h>
#include "AsyncFunctionCall.h"
#include <ShlGuid.h>
#include "logging.h"

#include "WinMessageWindow.h"

extern HINSTANCE gInstance;

FB::WinMessageWindow::WinMessageWindow() {
    WNDCLASSEX wc;
    DWORD err(0);
    static ATOM clsAtom(NULL);

    wchar_t *wszWinName = L"FireBreathEventWindow";
    wchar_t *wszClassName = L"FBEventWindow";

    if (!clsAtom) {
        //Step 1: Registering the Window Class
        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.style         = 0;
        wc.lpfnWndProc   = &WinMessageWindow::_WinProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = gInstance;
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = wszClassName;
        wc.hIcon = NULL;
        wc.hCursor = NULL;
        wc.hIconSm = NULL;
        wc.hbrBackground = NULL;
    
        if (!(clsAtom = ::RegisterClassEx(&wc))) {
            err = ::GetLastError();    
        }
    }
    // Step 2: Creating the Window
    HWND messageWin = CreateWindowEx(
        WS_OVERLAPPED,
        (LPCWSTR)clsAtom,
        wszWinName,
        0,
        0, 0, 0, 0,
        HWND_MESSAGE, NULL, gInstance, NULL);
    if (!messageWin) {
        err = ::GetLastError();
        throw std::runtime_error("Could not create Message Window");
    }
    m_hWnd = messageWin;
}

LRESULT CALLBACK FB::WinMessageWindow::_WinProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if (uMsg == WM_ASYNCTHREADINVOKE) {
        FBLOG_TRACE("PluginWindow", "Running async function call");
        FB::AsyncFunctionCall *evt = static_cast<FB::AsyncFunctionCall*>((void*)lParam);
        evt->func(evt->userData);
        delete evt;
        return S_OK;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND FB::WinMessageWindow::getHWND()
{
    return m_hWnd;
}

FB::WinMessageWindow::~WinMessageWindow()
{
    ::DestroyWindow(m_hWnd);
}
