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
        void setWindowPosition(long x, long y, long w, long h);
        void getWindowPosition(long &x, long &y, long &w, long &h) const;
        FB::Rect getWindowPosition() const;
        void setWindowClipping(long t, long l, long b, long r);
        void getWindowClipping(long &t, long &l, long &b, long &r) const;
        FB::Rect getWindowClipping() const;
        virtual void InvalidateWindow() const;
        long getWindowWidth() const { return m_width; }
        long getWindowHeight() const { return m_height; }

#if FB_GUI_DISABLED != 1
    public:
        GdkNativeWindow getWindow();
        void setBrowserWindow(GdkNativeWindow win) {  m_browserWindow = win; }
        GdkNativeWindow getBrowserWindow() { return m_browserWindow; }
        static gboolean _EventCallback(GtkWidget *widget, GdkEvent *event, gpointer user_data);
    protected:
        gboolean EventCallback(GtkWidget *widget, GdkEvent *event);

        GdkNativeWindow m_window;
        GdkNativeWindow m_browserWindow;
        int m_handler_id;
        GtkWidget *m_container;
        GtkWidget *m_canvas;

#endif

    protected:
        long m_x;
        long m_y;
        long m_width;
        long m_height;

        long m_clipLeft;
        long m_clipRight;
        long m_clipTop;
        long m_clipBottom;
    };

};

#endif // H_PLUGINWINDOWX11
