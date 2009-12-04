
#include "Win/FactoryDefinitionsWin.h"
#include "Win/PluginWindowWin.h"

FB::PluginWindowWin *_createPluginWindow(HWND hWnd)
{
    return new FB::PluginWindowWin(hWnd);
}