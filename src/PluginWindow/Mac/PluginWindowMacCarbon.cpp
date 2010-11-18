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

PluginWindowMacCarbon::PluginWindowMacCarbon()
{
    
}

PluginWindowMacCarbon::~PluginWindowMacCarbon()
{
    
}

void PluginWindowMacCarbon::setWindowPosition(int32_t x, int32_t y, uint32_t width, uint32_t height) {
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
}

void PluginWindowMacCarbon::setWindowClipping(uint16_t top, uint16_t left, uint16_t bottom, uint16_t right) {
    m_clipTop = top;
    m_clipLeft = left;
    m_clipBottom = bottom;
    m_clipRight = right;
    ClipChangedEvent ev;
    SendEvent(&ev);
}

NPRect PluginWindowMacCarbon::getWindowPosition() {
    NPRect windRect;
    windRect.left = m_x;
    windRect.top = m_y;
    windRect.right = m_x + m_width;
    windRect.bottom = m_y + m_height;
    return windRect;
}

NPRect PluginWindowMacCarbon::getWindowClipping() {
    NPRect clipRect;
    clipRect.left = m_clipLeft;
    clipRect.right = m_clipRight;
    clipRect.top = m_clipTop;
    clipRect.bottom = m_clipBottom;
    return clipRect;    
}

int PluginWindowMacCarbon::getWindowHeight() {
    return this->m_height;
}

int PluginWindowMacCarbon::getWindowWidth() {
    return this->m_width;
}

void PluginWindowMacCarbon::clearWindow() {
    // TODO
}
