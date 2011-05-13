/**********************************************************\
Original Author: Anson MacKeracher 

Created:    Mar 26, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Anson MacKeracher, Firebreath development team
\**********************************************************/

#ifndef H_PLUGINEVENTMACCOCOA
#define H_PLUGINEVENTMACCOCOA

#include "PluginEventMac.h"

namespace FB {
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  PluginEventMacCocoa
    ///
    /// @brief  Mac OS X Cocoa specific implementation of PluginEventMac
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class PluginEventMacCocoa : public PluginEventMac {
    public:
        PluginEventMacCocoa();
        virtual ~PluginEventMacCocoa();

        int16_t HandleEvent(void* event);
        EventModel getEventModel() const { return EventModelCocoa; }
        
    protected:
        int16_t ProcessModifiers(uint32_t modifierFlags);
        
    private:
        uint32_t m_lastModifierFlags;
    };
};

#endif // H_PLUGINEVENTMACCOCOA
