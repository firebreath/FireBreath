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
#include "PluginEvents/KeyboardEvents.h"
#include "ConstructDefaultPluginWindows.h"
#include "logging.h"
#include "X11/KeyCodesX11.h"

#include "PluginWindowX11.h"

#if FB_GUI_DISABLED != 1

#include <gdk/gdkx.h>

#endif

FB::PluginWindowX11* FB::createPluginWindowX11(const FB::WindowContextX11& ctx)
{
    return new FB::PluginWindowX11(ctx);
}

using namespace FB;

PluginWindowX11::PluginWindowX11(const WindowContextX11& ctx)
  :
#if FB_GUI_DISABLED != 1
    m_window(ctx.window),
    m_browserWindow(0),
    m_focus(false),
#endif
    m_x(0), m_y(0), m_width(0), m_height(0), m_clipLeft(0), m_clipRight(0),
    m_clipTop(0), m_clipBottom(0), m_handler_id(0)
{
    FBLOG_INFO("FB.PluginWindowX11", "Creating new PluginWindowX11");
#if FB_GUI_DISABLED != 1
    m_container = gtk_plug_new((GdkNativeWindow)ctx.window);
    m_canvas = gtk_drawing_area_new();
#ifdef GTK_WIDGET_SET_FLAGS
    GTK_WIDGET_SET_FLAGS(GTK_WIDGET(m_canvas), GTK_CAN_FOCUS);
#else // gtk+-2.22 or newer
    gtk_widget_set_can_focus(GTK_WIDGET(m_canvas), true);
#endif

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

    m_handler_id = g_signal_connect(G_OBJECT(m_canvas), "event", G_CALLBACK(&PluginWindowX11::_EventCallback), this);
    gtk_widget_show(m_canvas);
    gtk_container_add(GTK_CONTAINER(m_container), m_canvas);
    gtk_widget_show(m_container);
#endif
}

PluginWindowX11::~PluginWindowX11()
{
#if FB_GUI_DISABLED != 1
    g_signal_handler_disconnect(G_OBJECT(m_canvas), m_handler_id);
#endif
    FBLOG_INFO("FB.PluginWindowX11", "Destroying PluginWindowX11");
}

void PluginWindowX11::getWindowPosition(int32_t &x, int32_t &y, uint32_t &w, uint32_t &h) const
{
    x = m_x;
    y = m_y;
    w = m_width;
    h = m_height;
}

FB::Rect PluginWindowX11::getWindowPosition() const
{
    FB::Rect rect = {m_y, m_x, m_y+m_height, m_x+m_width};
    return rect;
}

void PluginWindowX11::setWindowPosition(int32_t x, int32_t y, uint32_t w, uint32_t h)
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

void PluginWindowX11::getWindowClipping(int32_t &t, int32_t &l, int32_t &b, int32_t &r) const
{
    t = m_clipTop;
    l = m_clipLeft;
    b = m_clipBottom;
    r = m_clipRight;
}

FB::Rect PluginWindowX11::getWindowClipping() const
{
    FB::Rect rect = {m_clipTop, m_clipLeft, m_clipBottom, m_clipRight};
    return rect;
}

void PluginWindowX11::setWindowClipping(int32_t t, int32_t l, int32_t b, int32_t r)
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

#if FB_GUI_DISABLED != 1

gboolean PluginWindowX11::_EventCallback(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    if (!user_data)
        return 0;

    PluginWindowX11 *pluginWin = (PluginWindowX11 *)user_data;
    return pluginWin->EventCallback(widget, event);
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

    unsigned int modifierState = 0;  //TODO
    switch(event->type)
    {
        // Mouse button down
        case GDK_BUTTON_PRESS: {
            MouseDownEvent evt(btn, button->x, button->y, modifierState);
            if(!m_focus){
                //When the mouse button is pressed, we can be sure,
                //that the top window has the focus and we can request keyboard focus.
                gtk_widget_grab_focus(widget);
            }
            return SendEvent(&evt) ? 1 : 0;
        } break;

        // Mouse button up
        case GDK_2BUTTON_PRESS: {
            MouseDoubleClickEvent evt(btn, button->x, button->y, modifierState);
            return SendEvent(&evt) ? 1 : 0;
        } break;
        // Mouse button up
        case GDK_BUTTON_RELEASE: {
            MouseUpEvent evt(btn, button->x, button->y, modifierState);
            return SendEvent(&evt) ? 1 : 0;
        } break;

        case GDK_KEY_PRESS: {
            GdkEventKey *key = (GdkEventKey *)event;
            KeyDownEvent evt(GDKKeyCodeToFBKeyCode(key->keyval),key->keyval);
            SendEvent(&evt) ? 1 : 0;
        } break;

        case GDK_KEY_RELEASE: {
            GdkEventKey *key = (GdkEventKey *)event;
            KeyUpEvent evt(GDKKeyCodeToFBKeyCode(key->keyval),key->keyval);
            SendEvent(&evt) ? 1 : 0;
        } break;

        case GDK_MOTION_NOTIFY: {
            GdkEventMotion *motion = (GdkEventMotion *)event;
            MouseMoveEvent evt(motion->x, motion->y);
            return SendEvent(&evt) ? 1 : 0;
        } break;

        case GDK_FOCUS_CHANGE: {
            GdkEventFocus *focus = (GdkEventFocus *)event;
            m_focus = focus->in ? true : false;
            FocusChangedEvent evt(m_focus);
            return SendEvent(&evt) ? 1 : 0;
        }
        default:
            return false;
    }

    return false;
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

GdkNativeWindow PluginWindowX11::getWindow()
{
#if GTK_CHECK_VERSION(2, 14, 0)
  return GDK_WINDOW_XID(gtk_widget_get_window(m_canvas));
#else
  return GDK_WINDOW_XID(GTK_WIDGET(m_canvas)->window);
#endif
}

#endif

void PluginWindowX11::InvalidateWindow() const
{
    // Doesn't exist yet
}

