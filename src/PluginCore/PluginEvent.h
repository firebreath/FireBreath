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

#ifndef H_FB_PLUGINEVENT
#define H_FB_PLUGINEVENT

#include <string>
#include <map>
#include <stdexcept>

namespace FB {

    /**
     * PluginEvent
     *
     * This is the base class for all OS events that are used in the plugin;
     * generally these come from the PluginWindow class (or a specialization
     * thereof)
     **/
    class PluginEvent
    {
    public:
        PluginEvent() { };
        virtual ~PluginEvent() { };

    public:

        template<class T>
        T* get()
        {
            T* out(dynamic_cast<T*>(this));
            if (out == NULL) {
                throw std::invalid_argument("Invalid message type");
            }
            return out;
        }

        template<class T>
        bool validType()
        {
            T* out(dynamic_cast<T*>(this));
            return out != NULL;
        }
    };
};

#endif
