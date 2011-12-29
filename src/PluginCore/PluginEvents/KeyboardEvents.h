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
        KeyEvent(FBKeyCode fb_key, uint32_t os_key, uint32_t modifiers = 0)
            :
        m_key_code( fb_key ), m_os_key_code( os_key ), m_modifierFlags(modifiers)
        { }

    public:
        FBKeyCode m_key_code;
        uint32_t m_os_key_code;
        uint32_t m_modifierFlags;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  KeyUpEvent
    ///
    /// @brief  Fired for a key up event
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class KeyUpEvent : public KeyEvent
    {
    public:
        KeyUpEvent(FBKeyCode fb_key, uint32_t os_key, uint32_t modifiers = 0)
            : KeyEvent(fb_key, os_key, modifiers) {}
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  KeyDownEvent
    ///
    /// @brief  Fired for a key down event
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class KeyDownEvent : public KeyEvent
    {
    public:
        KeyDownEvent(FBKeyCode fb_key, uint32_t os_key, uint32_t modifiers = 0)
            : KeyEvent(fb_key, os_key, modifiers) {}
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  TextEvent
    ///
    /// @brief  Used at least on Mac, possibly elsewhere; gives us text
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class TextEvent : public PluginEvent
    {
    public:
        TextEvent(const std::string& text) : text(text) {}
        
    public:
        std::string text;
    };
    
    
};

#endif // H_FB_PLUGINEVENTS_KEYBOARDEVENTS

