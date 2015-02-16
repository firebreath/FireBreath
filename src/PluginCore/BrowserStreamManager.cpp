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

#include "BrowserStream.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include <boost/thread/shared_mutex.hpp>
#include <algorithm>

#include "BrowserStreamManager.h"

FB::BrowserStreamManager::BrowserStreamManager()
{

}

FB::BrowserStreamManager::~BrowserStreamManager()
{
    std::unique_lock<std::recursive_mutex> _l(m_xtmutex);

    // Force close all of the streams so that they get a callback
    for (auto stream : m_retainedStreams) {
        stream->close();
    }
    m_retainedStreams.clear();
}

void FB::BrowserStreamManager::retainStream( const BrowserStreamPtr& stream )
{
    std::unique_lock<std::recursive_mutex> _l(m_xtmutex);
    stream->AttachObserver(shared_from_this());
    m_retainedStreams.insert(stream);
}

void FB::BrowserStreamManager::releaseStream( const BrowserStreamPtr& stream )
{
    std::unique_lock<std::recursive_mutex> _l(m_xtmutex);
    stream->DetachObserver(shared_from_this());
    m_retainedStreams.erase(stream);
}

bool FB::BrowserStreamManager::onStreamCompleted( FB::StreamCompletedEvent *evt, FB::BrowserStream *stream )
{
    releaseStream(std::dynamic_pointer_cast<FB::BrowserStream>(stream->shared_from_this()));
    return false;
}

