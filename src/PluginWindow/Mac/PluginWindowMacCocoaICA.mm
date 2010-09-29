#include "PluginWindowMacCocoaICA.h"

using namespace FB;

PluginWindowMacCocoaICA::PluginWindowMacCocoaICA() {}

PluginWindowMacCocoaICA::~PluginWindowMacCocoaICA() {}

void PluginWindowMacCocoaICA::InvalidateWindow() {
    NPRect rect = getWindowPosition();
    m_npHost->InvalidateRect(&rect); 
}
