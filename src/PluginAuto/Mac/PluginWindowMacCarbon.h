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

#ifndef H_PLUGINWINDOWMAC_CARBON
#define H_PLUGINWINDOWMAC_CARBON

#include "PluginWindow.h"
#include <map>
#include "PluginEvents/MacEventCarbon.h"
#include "PluginEvents/DrawingEvents.h"
#include "NpapiTypes.h"
#include <ApplicationServices/ApplicationServices.h>
#include "NpapiBrowserHost.h"

namespace FB {
    
    class PluginWindowMacCarbon : public PluginWindow
    {
    public:
        PluginWindowMacCarbon();
        virtual ~PluginWindowMacCarbon();
        
        virtual int16_t HandleEvent(EventRecord* evt) { return 0; }
        virtual void InvalidateWindow() {/* TODO */}
        virtual void clearWindow();

        // Accessors 
		FB::Rect getWindowPosition() const;
		FB::Rect getWindowClipping() const;
        int getWindowHeight() const;
        int getWindowWidth() const;

        void setWindowPosition(int32_t x, int32_t y, uint32_t width, uint32_t height);
        void setWindowClipping(uint16_t top, uint16_t left, uint16_t bottom, uint16_t right);

        // Set this window's browser host
        void setNpHost(FB::Npapi::NpapiBrowserHostPtr host) { m_npHost = host; }

    protected:
        int m_x;
        int m_y;
        int m_width;
        int m_height;
        
        int m_clipLeft;
        int m_clipRight;
        int m_clipTop;
        int m_clipBottom;

        int m_old_x, m_old_y; // Keep track of mouse movement coordinates

        Npapi::NpapiBrowserHostPtr m_npHost;
    };
};

#endif // H_PLUGINWINDOWMAC_CARBON
