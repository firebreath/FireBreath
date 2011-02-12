#pragma once
#ifndef BrowserStreamManager_h__
#define BrowserStreamManager_h__

/**********************************************************\
Original Author: Richard Bateman

Created:    Jan 25, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2011 Richard Bateman, 
               Firebreath development team
\**********************************************************/

#include <set>
#include <boost/noncopyable.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include "PluginEventSink.h"
#include "PluginEvents/StreamEvents.h"
#include "BrowserStream.h"
#include "FBPointers.h"

namespace FB {

    class BrowserStreamManager : public FB::PluginEventSink
    {
    public:
        BrowserStreamManager();
        ~BrowserStreamManager();

        void retainStream(const BrowserStreamPtr& stream);
        void releaseStream(const BrowserStreamPtr& stream);

        BEGIN_PLUGIN_EVENT_MAP()
            EVENTTYPE_CASE(FB::StreamCompletedEvent, onStreamCompleted, FB::BrowserStream)
        END_PLUGIN_EVENT_MAP()
        virtual bool onStreamCompleted(FB::StreamCompletedEvent *evt, FB::BrowserStream *stream);
    private:
        std::set<BrowserStreamPtr> m_retainedStreams;
        mutable boost::recursive_mutex m_xtmutex;
    };

};
#endif // BrowserStreamManager_h__

