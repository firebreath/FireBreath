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

#ifndef H_PLUGINEVENTMAC
#define H_PLUGINEVENTMAC

#include "NpapiBrowserHost.h"
#include "NpapiTypes.h"
#include "FBPointers.h"

namespace FB {
    
    FB_FORWARD_PTR(PluginEventMac);
    FB_FORWARD_PTR(PluginWindowMac);

    class PluginEventMac
    {
    public:
        enum EventModel
        {
            EventModelCarbon,
            EventModelCocoa
        };
        static NPEventModel initPluginEventMac(const FB::Npapi::NpapiBrowserHostPtr &host, NPDrawingModel drawingModel);
        static FB::PluginEventMac* createPluginEventMac(NPEventModel eventModel);

        PluginEventMac() {}
        virtual ~PluginEventMac() {}

        virtual EventModel getEventModel() const = 0;
        virtual int16_t HandleEvent(void* event) = 0;
        void setPluginWindow(PluginWindowMacPtr window) { m_PluginWindow = window; }
    
    protected:
        PluginWindowMacWeakPtr m_PluginWindow;
    };

};

#endif // H_PLUGINEVENTMAC
