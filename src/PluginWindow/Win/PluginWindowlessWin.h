/**********************************************************\ 
Original Author: Anson MacKeracher (amackera) 

Created:    Aug 5, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Anson MacKeracher, Firebreath development team
\**********************************************************/

#ifndef H_PLUGINWINDOWLESSWIN
#define H_PLUGINWINDOWLESSWIN
#include "Win/win_common.h"
#include "PluginWindow.h"

#include "NpapiBrowserHost.h"
#include "NpapiPlugin.h"
#include "Win/KeyCodesWin.h"

#include "PluginEvents/WindowsEvent.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h" 
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/KeyboardEvents.h"

namespace FB {
    class PluginWindowlessWin : public PluginWindow
    {
        public:
            PluginWindowlessWin(HDC hdc);
            ~PluginWindowlessWin();
            
            HDC getHDC() { return m_hdc; }
            void setHDC(HDC hdc) { m_hdc = hdc; }
            HWND getHWND();

            Npapi::NpapiBrowserHostPtr getNpHost() { return m_npHost; }
            void setNpHost(Npapi::NpapiBrowserHostPtr npHost) { m_npHost = npHost; }

            NPRect getWindowPosition();
            void setWindowPosition(int x, int y, int width, int height);
            void setWindowPosition(NPRect pos);

            NPRect getWindowClipping();
            void setWindowClipping(int top, int left, int bottom, int right);
            void setWindowClipping(NPRect clip);

            int getWindowWidth() { return m_width; }
            int getWindowHeight() { return m_height; }

            int16_t HandleEvent(NPEvent* evt);

            void InvalidateWindow();
            void AsyncInvalidateWindow();

        protected:
            HDC m_hdc;
            Npapi::NpapiBrowserHostPtr m_npHost;
            int m_x, m_y, m_width, m_height; 
            int m_clipTop, m_clipLeft, m_clipBottom, m_clipRight;
    };    
};

#endif
