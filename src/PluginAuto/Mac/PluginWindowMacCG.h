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

#ifndef H_PLUGINWINDOWMACCG
#define H_PLUGINWINDOWMACCG

#include <Carbon/Carbon.h>
#include "PluginWindowMac.h"

namespace FB {
    class PluginWindowMacCG : public PluginWindowMac {
    public:
        PluginWindowMacCG();
        virtual ~PluginWindowMacCG();

        NPError SetWindow(NPWindow* window);

        DrawingModel getDrawingModel() const { return DrawingModelCoreGraphics; }
        void* getDrawingPrimitive() const { return m_cgContext.context; }
        WindowRef getWindowRef() const { return (WindowRef) m_cgContext.window; }

        bool SendEvent(PluginEvent* evt);

    protected:
        void DrawLabel(CGContextRef cgContext, FB::Rect clip);

    public:
        NP_CGContext m_cgContext;
        uint64_t m_count;
        std::list<uint64_t> m_frames;
    };
};

#endif // H_PLUGINWINDOWMACCG
