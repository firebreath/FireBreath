#include "PluginWindowMacCocoaICA.h"

using namespace FB;

PluginWindowMacCocoaICA::PluginWindowMacCocoaICA() {}

PluginWindowMacCocoaICA::~PluginWindowMacCocoaICA() {}

void PluginWindowMacCocoaICA::InvalidateWindow() {
    m_npHost->InvalidateRect(&getWindowPosition()); 
}
