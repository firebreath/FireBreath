/**********************************************************\
Original Author: Anson MacKeracher 

Created:    May 5, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Anson MacKeracher, Firebreath development team
\**********************************************************/

#ifndef H_PLUGINWINDOWMAC_COCOA_CG
#define H_PLUGINWINDOWMAC_COCOA_CG

#include "PluginWindowMacCocoa.h"
#include "PluginEvents/MacEventCocoa.h"
#include "NpapiTypes.h"
#include <map>

namespace FB {
    class PluginWindowMacCocoaCG : public PluginWindowMacCocoa {
        public:
            PluginWindowMacCocoaCG();
            ~PluginWindowMacCocoaCG();

        public:
            void clearWindow();

            virtual int16_t HandleEvent(NPCocoaEvent* evt);

            // Handle the CoreGraphics drawing contexts
            void setContext(CGContextRef context);
            CGContextRef getContext() { return m_context; }

            void setWindowPosition(int32_t x, int32_t y, uint32_t width, uint32_t height);
            void setWindowClipping(uint32_t top, uint32_t left, uint32_t bottom, uint32_t right);
            NPRect getWindowPosition();
            NPRect getWindowClipping();

        protected:
            CGContextRef m_context;
    };
};

#endif // H_PLUGINWINDOWMAC_COCOA_CG
