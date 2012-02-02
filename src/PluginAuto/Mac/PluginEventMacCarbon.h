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

#ifndef H_PLUGINEVENTMACCARBON
#define H_PLUGINEVENTMACCARBON

#ifndef NP_NO_CARBON

#include "PluginEventMac.h"

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  PluginEventMacCarbon
    ///
    /// @brief  Mac OS X Carbon specific implementation of PluginEventMac
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class PluginEventMacCarbon : public PluginEventMac
    {
    public:
        PluginEventMacCarbon();
        virtual ~PluginEventMacCarbon();

        int16_t HandleEvent(void* event);
        EventModel getEventModel() const { return EventModelCarbon; }

    protected:
        Point GlobalToLocal(Point location);
        bool isMouseOver(Point location);

        void CarbonToNPCocoaEvent(const EventRecord* evt, NPCocoaEvent& rval);
        NPNSString* TranslateKeyEventRecord(const EventRecord *event);

        short m_old_h, m_old_v; // Keep track of mouse movement coordinates
        bool m_mouseEntered; // Keep track of mouseEntered/mouseExited
        CFDataRef m_tisKeyLayoutData;
        UInt32 m_deadKeyState;
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 1050
        TISInputSourceRef m_tisInputSource;
#endif
    };
};

#endif // NP_NO_CARBON

#endif // H_PLUGINEVENTMACCARBON
