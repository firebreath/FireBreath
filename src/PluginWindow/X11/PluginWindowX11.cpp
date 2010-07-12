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

#include "PluginEvents/X11Event.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginWindowX11.h"

using namespace FB;

gboolean PluginWindowX11::_EventCallback(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    if (!user_data)
        return 0;

    PluginWindowX11 *pluginWin = (PluginWindowX11 *)user_data;
    return pluginWin->EventCallback(widget, event);
}

PluginWindowX11::PluginWindowX11(GdkNativeWindow win) : m_window(win),
    /* We as C++ programmers do not usually care about initializing integers
     * when it's not actually needed, but it's nice for the code to be clean
     * enough to pass valgrinds memcheck. Without initializing, valgrind
     * will utter errors like:
     * "Conditional jump or move depends on uninitialised value(s)"
     * in setWindowPosition and setWindowClipping
     */
    m_x(0), m_y(0), m_width(0), m_height(0), m_clipLeft(0), m_clipRight(0),
    m_clipTop(0), m_clipBottom(0)
{
    m_window = win;
    m_container = gtk_plug_new((GdkNativeWindow)win);
    m_canvas = gtk_drawing_area_new();
    GTK_WIDGET_SET_FLAGS (GTK_WIDGET(m_canvas), GTK_CAN_FOCUS);

    gtk_widget_add_events(
        m_canvas,
        GDK_BUTTON_PRESS_MASK |
        GDK_BUTTON_RELEASE_MASK |
        GDK_KEY_PRESS_MASK |
        GDK_KEY_RELEASE_MASK |
        GDK_POINTER_MOTION_MASK |
        GDK_POINTER_MOTION_HINT_MASK |
        GDK_SCROLL_MASK |
        GDK_EXPOSURE_MASK |
        GDK_VISIBILITY_NOTIFY_MASK |
        GDK_ENTER_NOTIFY_MASK |
        GDK_LEAVE_NOTIFY_MASK |
        GDK_FOCUS_CHANGE_MASK
    );

    g_signal_connect(G_OBJECT(m_canvas), "event", G_CALLBACK(&PluginWindowX11::_EventCallback), this);
    gtk_widget_show(m_canvas);
    gtk_container_add(GTK_CONTAINER(m_container), m_canvas);
    gtk_widget_show(m_container);
}

PluginWindowX11::~PluginWindowX11()
{
}

void PluginWindowX11::getWindowPosition(int &x, int &y, int &w, int &h)
{
    x = m_x;
    y = m_y;
    w = m_width;
    h = m_height;
}

void PluginWindowX11::setWindowPosition(int x, int y, int w, int h)
{
    if (m_x != x
        || m_y != y
        || m_width != w
        || m_height != h) {
        m_x = x;
        m_y = y;
        m_width = w;
        m_height = h;
        ResizedEvent evt;
        SendEvent(&evt);
    }
}

void PluginWindowX11::getWindowClipping(int &t, int &l, int &b, int &r)
{
    t = m_clipTop;
    l = m_clipLeft;
    b = m_clipBottom;
    r = m_clipRight;
}

void PluginWindowX11::setWindowClipping(int t, int l, int b, int r)
{
    if (m_clipTop != t
        || m_clipLeft != l
        || m_clipBottom != b
        || m_clipRight != r) {
        m_clipTop = t;
        m_clipLeft = l;
        m_clipBottom = b;
        m_clipRight = r;
        ClipChangedEvent evt;
        SendEvent(&evt);
    }
}

inline bool isButtonEvent(GdkEvent *event)
{
    switch(event->type) {
        case GDK_BUTTON_PRESS:
        case GDK_BUTTON_RELEASE:
        case GDK_2BUTTON_PRESS:
        case GDK_3BUTTON_PRESS:
            return true;
        default:
            return false;
    }
}

gboolean PluginWindowX11::EventCallback(GtkWidget *widget, GdkEvent *event)
{
    X11Event ev(widget, event);
    if (SendEvent(&ev)) {
        return true;
    }

    gboolean handled = 0;
    GdkEventKey *key;
    GdkEventFocus *focus;
    GdkEventMotion *motion;
    GdkEventButton *button;
    MouseButtonEvent::MouseButton btn;

    if (isButtonEvent(event)) {
        button = (GdkEventButton *)event;

        switch(button->button) {
            case 1:
                btn = MouseButtonEvent::MouseButton_Left;
                break;
            case 2:
                btn = MouseButtonEvent::MouseButton_Middle;
                break;
            case 3:
                btn = MouseButtonEvent::MouseButton_Right;
                break;
            default:
                return 0;
        }
    }

    switch(event->type)
    {
        // Mouse button down
        case GDK_BUTTON_PRESS: {
            MouseDownEvent evt(btn, button->x, button->y);
            return SendEvent(&evt) ? 1 : 0;
        } break;
        // Mouse button up
        case GDK_BUTTON_RELEASE: {
            MouseUpEvent evt(btn, button->x, button->y);
            return SendEvent(&evt) ? 1 : 0;
        } break;

        case GDK_MOTION_NOTIFY: {
            motion = (GdkEventMotion *)event;
            MouseMoveEvent evt(motion->x, motion->y);
            return SendEvent(&evt) ? 1 : 0;
        } break;

        case GDK_FOCUS_CHANGE: {
            focus = (GdkEventFocus *)event;
            FocusChangedEvent evt(focus->in ? true : false);
            return SendEvent(&evt) ? 1 : 0;
        }
        default:
            return false;
    }
}

//
// int16_t PluginWindowX11::HandleEvent(EventRecord* evt)
// {
//     // Give the plugin a change to handle the event itself if desired
//     if (SendEvent(&MacEvent(evt))) {
//         return true;
//     }
//     switch (evt->what) {
//         case updateEvt:
//             return SendEvent(&RefreshEvent());
//             break;
//         default:
//             break;
//     }
//     return false;
// }

void PluginWindowX11::InvalidateWindow()
{
    // Doesn't exist yet
}
