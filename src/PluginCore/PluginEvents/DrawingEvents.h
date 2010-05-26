/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Dec 7, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_PLUGINEVENTS_DRAWINGEVENTS
#define H_FB_PLUGINEVENTS_DRAWINGEVENTS

#include "PluginEvent.h"

namespace FB {

    class ResizedEvent : public PluginEvent
    {
    };

    class ClipChangedEvent : public PluginEvent
    {
    };

    class RefreshEvent : public PluginEvent
    {
    };

    class FocusChangedEvent : public PluginEvent
    {
        public:
            FocusChangedEvent(bool hasFocus) : m_hasFocus(hasFocus) { }
            virtual ~FocusChangedEvent() { }

            bool hasFocus() { return m_hasFocus; }
        protected:
            bool m_hasFocus;
    };

};

#endif // H_FB_PLUGINEVENTS_DRAWINGEVENTS
