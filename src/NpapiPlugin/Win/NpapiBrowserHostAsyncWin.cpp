
#include "NpapiBrowserHostAsyncWin.h"
#include "Win/PluginWindowWin.h"

using namespace FB::Npapi;

NpapiBrowserHostAsyncWin::NpapiBrowserHostAsyncWin(NpapiPluginModule* module, NPP npp)
  : NpapiBrowserHost(module, npp)
  , m_hwnd(0)
{

}

NpapiBrowserHostAsyncWin::~NpapiBrowserHostAsyncWin()
{

}

void NpapiBrowserHostAsyncWin::setWindow(NPWindow* window)
{
    if(!window) {
        m_hwnd = 0;
        return;
    }

    m_hwnd = static_cast<HWND>(window->window);
}

void NpapiBrowserHostAsyncWin::ScheduleAsyncCall(void (*func)(void*), void* userData)
{
    if(!m_hwnd) 
        return;
    
    ::PostMessage(m_hwnd, WM_ASYNCTHREADINVOKE, NULL, 
                  (LPARAM)new FB::WINDOWS_ASYNC_EVENT(func, userData));
}