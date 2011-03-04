/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Nov 24, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_PLUGINWINDOWX11
#define H_PLUGINWINDOWX11

#include "global/config.h"

#if FB_GUI_DISABLED != 1

#include <X11/Xlib.h>
#include <gtk/gtk.h>

#endif

#include "PluginWindow.h"
#include "WindowContextX11.h"

#include <map>

namespace FB {

    class PluginWindowX11 : public PluginWindow
    {
    public:
        PluginWindowX11(const WindowContextX11&);
        virtual ~PluginWindowX11();

//         int16_t HandleEvent(EventRecord* evt);
        void setWindowPosition(int32_t x, int32_t y, uint32_t w, uint32_t h);
        void getWindowPosition(int32_t &x, int32_t &y, uint32_t &w, uint32_t &h) const;
        FB::Rect getWindowPosition() const;
        void setWindowClipping(int32_t t, int32_t l, int32_t b, int32_t r);
        void getWindowClipping(int32_t &t, int32_t &l, int32_t &b, int32_t &r) const;
        FB::Rect getWindowClipping() const;
        virtual void InvalidateWindow() const;
        uint32_t getWindowWidth() const { return m_width; }
        uint32_t getWindowHeight() const { return m_height; }

#if FB_GUI_DISABLED != 1
    public:
        GdkNativeWindow getWindow();
        GtkWidget* getWidget() { return m_canvas; }
        void setBrowserWindow(GdkNativeWindow win) {  m_browserWindow = win; }
        GdkNativeWindow getBrowserWindow() { return m_browserWindow; }
        static gboolean _EventCallback(GtkWidget *widget, GdkEvent *event, gpointer user_data);

        // You probably won't ever want to call this yourself.  Call getWindow instead.
        GdkNativeWindow getTopLevelWindow() { return m_window; }
    protected:
        gboolean EventCallback(GtkWidget *widget, GdkEvent *event);

        GdkNativeWindow m_window;
        GdkNativeWindow m_browserWindow;
        GtkWidget *m_container;
        GtkWidget *m_canvas;

#endif

    protected:
        int m_handler_id;
        int32_t m_x;
        int32_t m_y;
        uint32_t m_width;
        uint32_t m_height;

        int32_t m_clipLeft;
        int32_t m_clipRight;
        int32_t m_clipTop;
        int32_t m_clipBottom;
    };

};

#endif // H_PLUGINWINDOWX11

