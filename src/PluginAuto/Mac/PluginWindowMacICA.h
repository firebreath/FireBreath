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

#ifndef H_PLUGINWINDOWMACICA
#define H_PLUGINWINDOWMACICA

#include "PluginWindowMacCA.h"

namespace FB {
    class PluginWindowMacICA: public PluginWindowMacCA
    {
    public:
        PluginWindowMacICA();
        virtual ~PluginWindowMacICA() {}

        DrawingModel getDrawingModel() const { return DrawingModelInvalidatingCoreAnimation; }
        
        bool displayOnInvalidate() const { return m_displayOnInvalidate; }
        void setDisplayOnInvalidate(bool value) { m_displayOnInvalidate = value; }
        
        void InvalidateWindow() const;

    private:
        bool m_displayOnInvalidate;
    };
};
#endif // H_PLUGINWINDOWMACICA
