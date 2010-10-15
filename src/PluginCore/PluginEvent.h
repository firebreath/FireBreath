/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_PLUGINEVENT
#define H_FB_PLUGINEVENT

#include <string>
#include <map>
#include <stdexcept>

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  PluginEvent
    ///
    /// @brief  Plugin event base class. 
    ///
    /// This is the base class for all system (non-js) events that are used in the plugin.  Most of
    /// these come from the PluginWindow, but BrowserStream uses these as well, and in the future other
    /// functionality might.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class PluginEvent
    {
    public:
        PluginEvent() { };
        virtual ~PluginEvent() { };

    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template<class T> T* PluginEvent::get()
        ///
        /// @brief  Dynamic cast the pluginEvent to the specified type and throw an exception if it is not
        /// 		a subtype of the specified type
        /// 		
        /// @code
        /// 	 AttachedEvent *atevt = evt->get<AttachedEvent>();
        /// @endcode
        ///
        /// @exception  std::invalid_argument   Thrown when invalid argument. 
        ///
        /// @return null if it fails, else a T* 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template<class T>
        T* get()
        {
            T* out(dynamic_cast<T*>(this));
            if (out == NULL) {
                throw std::invalid_argument("Invalid message type");
            }
            return out;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template<class T> bool PluginEvent::validType()
        ///
        /// @brief  Returns true if the object can be cast to the specified type
        ///
        /// @return true if the type is valid, false if not 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template<class T>
        bool validType()
        {
            T* out(dynamic_cast<T*>(this));
            return out != NULL;
        }
    };
};

#endif
