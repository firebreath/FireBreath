/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    April 2, 2010
License:    Dual license model; choose one of two:
New BSD License
http://www.opensource.org/licenses/bsd-license.php
- or -
GNU Lesser General Public License, version 2.1
http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_PLUGINEVENTS_KEYBOARDEVENTS
#define H_FB_PLUGINEVENTS_KEYBOARDEVENTS

#include "PluginEvent.h"
#include "KeyCodes.h"

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  KeyEvent
    ///
    /// @brief  Fired for a key event
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class KeyEvent : public PluginEvent
    {
    public:
        KeyEvent(FBKeyCode fb_key, unsigned int os_key)
            :
        m_key_code( fb_key ), m_os_key_code( os_key )
        { }

    public:
        FBKeyCode m_key_code;
        unsigned int m_os_key_code;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  KeyUpEvent
    ///
    /// @brief  Fired for a key up event
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class KeyUpEvent : public KeyEvent
    {
    public:
        KeyUpEvent(FBKeyCode fb_key, unsigned int os_key) : KeyEvent(fb_key, os_key) {}
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  KeyDownEvent
    ///
    /// @brief  Fired for a key down event
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class KeyDownEvent : public KeyEvent
    {
    public:
        KeyDownEvent(FBKeyCode fb_key, unsigned int os_key) :   KeyEvent(fb_key, os_key) {}
    };

};

#endif // H_FB_PLUGINEVENTS_KEYBOARDEVENTS

