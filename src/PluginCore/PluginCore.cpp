
#include "PluginWindow.h"

#include "PluginCore.h"

using namespace FB;

/***************************\
 Static initialization stuff
\***************************/

volatile int PluginCore::ActivePluginCount = 0;

std::string PluginCore::OS;
std::string PluginCore::Browser;
void PluginCore::setPlatform(std::string os, std::string browser)
{
    PluginCore::OS = os;
    PluginCore::Browser = browser;
}

/***************************\
     Regular Class Stuff
\***************************/

PluginCore::PluginCore() : m_Window(NULL)
{
    // This class is only created on the main UI thread,
    // so there is no need for mutexes here
    if (++PluginCore::ActivePluginCount == 1) {
        // Only on the first initialization
        StaticInitialize();
    }
}

PluginCore::~PluginCore()
{


    // This class is only destroyed on the main UI thread,
    // so there is no need for mutexes here
    if (--PluginCore::ActivePluginCount == 0) {
        // Only on the destruction of the final plugin instance
        StaticDeinitialize();
    }
}

void PluginCore::SetWindow(PluginWindow *wind)
{
    if (m_Window && m_Window != wind) {
        ClearWindow();
    }
    m_Window = wind;
    wind->AttachObserver(this);
}

void PluginCore::ClearWindow()
{
    if (m_Window) {
        m_Window->DetachObserver(this);
        m_Window = NULL;
    }
}