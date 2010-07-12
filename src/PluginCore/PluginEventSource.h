/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#ifndef H_FB_PLUGINEVENTSOURCE
#define H_FB_PLUGINEVENTSOURCE

#include <vector>
#include <typeinfo>

namespace FB {

    class PluginEventSink;
    class PluginEvent;

    /**
     * PluginWindow
     *
     * This is the base class for the window of a user-defined Plugin
     **/
    class PluginEventSource
    {
    public:
        PluginEventSource();
        virtual ~PluginEventSource();

        virtual void AttachObserver(PluginEventSink*);
        virtual void DetachObserver(PluginEventSink*);
        virtual bool SendEvent(PluginEvent*);

        template <class T>
        T* get_as()
        {
            T* tmp = dynamic_cast<T*>(this);
            if (!tmp)
                throw std::bad_cast();
            return tmp;
        }

    protected:
        typedef std::vector<PluginEventSink*> ObserverMap;
        ObserverMap m_observers;
    };
};

#endif
