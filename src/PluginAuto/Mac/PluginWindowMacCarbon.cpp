/**********************************************************\
 Original Author: Anson MacKeracher 
 
 Created:    Jul 12, 2010
 License:    Dual license model; choose one of two:
             New BSD License
             http://www.opensource.org/licenses/bsd-license.php
             - or -
             GNU Lesser General Public License, version 2.1
             http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 2010 Anson MacKeracher, Firebreath development team
 \**********************************************************/

#include "PluginWindowMacCarbon.h"

using namespace FB;

PluginWindowMacCarbon::PluginWindowMacCarbon() :
    m_x(0), m_y(0), m_width(0), m_height(0),
    m_clipLeft(0), m_clipRight(0), m_clipTop(0), m_clipBottom(0), 
    m_old_x(0), m_old_y(0)
{
}

PluginWindowMacCarbon::~PluginWindowMacCarbon() {}

void PluginWindowMacCarbon::setWindowPosition(int32_t x, int32_t y, uint32_t width, uint32_t height) {
    if (m_x != x
        || m_y != y
        || m_width != width
        || m_height != height) {
        m_x = x;
        m_y = y;
        m_width = width;
        m_height = height;
        ResizedEvent ev;
        SendEvent(&ev);
    }
}

void PluginWindowMacCarbon::setWindowClipping(uint32_t top, uint32_t left, uint32_t bottom, uint32_t right) {
    m_clipTop = top;
    m_clipLeft = left;
    m_clipBottom = bottom;
    m_clipRight = right;
    ClipChangedEvent ev;
    SendEvent(&ev);
}

FB::Rect PluginWindowMacCarbon::getWindowPosition() const {
	FB::Rect windRect;
    windRect.left = m_x;
    windRect.top = m_y;
    windRect.right = m_x + m_width;
    windRect.bottom = m_y + m_height;
    return windRect;
}

FB::Rect PluginWindowMacCarbon::getWindowClipping() const {
	FB::Rect clipRect;
    clipRect.left = m_clipLeft;
    clipRect.right = m_clipRight;
    clipRect.top = m_clipTop;
    clipRect.bottom = m_clipBottom;
    return clipRect;    
}

uint32_t PluginWindowMacCarbon::getWindowHeight() const {
    return m_height;
}

uint32_t PluginWindowMacCarbon::getWindowWidth() const {
    return m_width;
}

void PluginWindowMacCarbon::InvalidateWindow() const {
    FB::Rect pos = getWindowPosition();
    NPRect r = { pos.top, pos.left, pos.bottom, pos.right };
    if (!m_npHost->isMainThread()) {
        m_npHost->ScheduleOnMainThread(m_npHost, boost::bind(&Npapi::NpapiBrowserHost::InvalidateRect2, m_npHost, r));
    } else {
        m_npHost->InvalidateRect(&r);
    }
}

void PluginWindowMacCarbon::clearWindow() {
    // TODO
}

