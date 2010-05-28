#ifndef H_FB_EVENTS_X11EVENT
#define H_FB_EVENTS_X11EVENT

#include <X11/Xlib.h>
#include <gtk/gtk.h>

#include "PluginEvent.h"

namespace FB {

    class X11Event : public PluginEvent
    {
    public:
        X11Event(GtkWidget *widget, GdkEvent *event) : m_widget(widget), m_event(event)
        {
        }

    public:
        GtkWidget *m_widget;
        GdkEvent *m_event;
    };
};

#endif
