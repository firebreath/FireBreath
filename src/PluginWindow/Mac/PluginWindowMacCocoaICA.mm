#include "PluginWindowMacCocoaICA.h"

using namespace FB;

PluginWindowMacCocoaICA::PluginWindowMacCocoaICA() {}

PluginWindowMacCocoaICA::~PluginWindowMacCocoaICA() {}

void PluginWindowMacCocoaICA::InvalidateWindow() {
    // In plugin coordinates, invalidates entire window
    NPRect rect = { 0, 0, m_height, m_width};
    m_npHost->InvalidateRect(&rect); 
}
