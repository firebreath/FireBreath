
#include "Win/FactoryDefinitionsWin.h"
#include "Win/NpapiPluginWin.h"
#include "Win/PluginWindowWin.h"

FB::Npapi::NpapiPlugin *_getNpapiPlugin(FB::Npapi::NpapiBrowserHost *host)
{
    return new FB::Npapi::NpapiPluginWin(host);
}

FB::PluginWindowWin *_createPluginWindow(HWND hWnd)
{
    return new FB::PluginWindowWin(hWnd);
}