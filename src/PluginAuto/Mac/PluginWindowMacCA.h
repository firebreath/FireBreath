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

#ifndef H_PLUGINWINDOWMACCA
#define H_PLUGINWINDOWMACCA

#include <QuartzCore/QuartzCore.h>
#include "PluginWindowMac.h"

namespace FB
    {
    class PluginWindowMacCA: public PluginWindowMac
    {
    public:
        PluginWindowMacCA(bool invalidating);
        virtual ~PluginWindowMacCA();

        NPError SetWindow(NPWindow* window);

        DrawingModel getDrawingModel() const { return DrawingModelCoreAnimation; }
        void* getDrawingPrimitive() const { return m_layer; }
        WindowRef getWindowRef() const { return NULL; }

        void InvalidateWindow() const;

    private:
        void* m_layer;
        bool m_invalidating;        
    };
};
#endif // H_PLUGINWINDOWMACCA
