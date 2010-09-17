#include "PluginWindowMacCocoaICA.h"

PluginWindowMacCocoaICA::PluginWindowMacCocoaICA() {}

PluginWindowMacCocoaICA::~PluginWindowMacCocoaIC() {}

PluginWindowMacCocoaICA::InvalidateWindow() {
    m_host->InvalidateRect(getWindowPosition()); 
}
